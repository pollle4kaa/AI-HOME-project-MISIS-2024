import asyncio
from aiogram import Bot, Dispatcher, types, F
from aiogram.fsm.state import State, StatesGroup
from aiogram.fsm.context import FSMContext
from aiogram.fsm.storage.memory import MemoryStorage
from aiogram.types import InlineKeyboardMarkup, InlineKeyboardButton
from main import ya_daun

# Создаем бота и диспетчер
bot = Bot(token="7257385948:AAFA5D0XrkjeYXsGBiJS83hNjwaKGVm_doE")
dp = Dispatcher(storage=MemoryStorage())

# Определяем состояния
class Form(StatesGroup):
    area = State()
    floor = State()
    rooms = State()
    finishing = State()
    construction_date = State()
    metro = State()
    metro_time = State()
    park_time = State()
    school_time = State()
    clinic_time = State()
    kindergarten_time = State()
    parking = State()

# Кнопка "Начать сначала"
def start_over_button():
    return InlineKeyboardMarkup(
        inline_keyboard=[[InlineKeyboardButton(text="Начать сначала", callback_data="start_over")]]
    )

# Обработчик команды /start
@dp.message(F.text == "/start")
async def start_handler(message: types.Message, state: FSMContext):
    await state.clear()  # Очищаем состояние
    await state.set_state(Form.area)  # Устанавливаем состояние
    await message.answer("Введите диапазон площади в м^2 (например, 30 50):", reply_markup=start_over_button())

@dp.callback_query(F.data == "start_over")
async def restart_handler(callback: types.CallbackQuery, state: FSMContext):
    await callback.message.edit_text("Начинаем сначала...")
    await start_handler(callback.message, state)

@dp.message(Form.area)
async def process_area(message: types.Message, state: FSMContext):
    try:
        area_input = message.text.split()
        if len(area_input) != 2:
            raise ValueError("Введите два числа через пробел.")
        area_from, area_to = map(int, area_input)
        await state.update_data(area_from=area_from, area_to=area_to)
        await state.set_state(Form.floor)
        await message.answer("Введите диапазон этажей (например, 1 5):", reply_markup=start_over_button())
    except ValueError:
        await message.answer("Пожалуйста, введите два числа через пробел для площади.")

@dp.message(Form.floor)
async def process_floor(message: types.Message, state: FSMContext):
    try:
        floor_input = message.text.split()
        if len(floor_input) != 2:
            raise ValueError("Введите два числа через пробел.")
        floor_from, floor_to = map(int, floor_input)
        await state.update_data(floor_from=floor_from, floor_to=floor_to)
        await state.set_state(Form.rooms)
        await message.answer("Введите диапазон количества комнат (например, 1 3):", reply_markup=start_over_button())
    except ValueError:
        await message.answer("Пожалуйста, введите два числа через пробел для этажей.")

@dp.message(Form.rooms)
async def process_rooms(message: types.Message, state: FSMContext):
    try:
        rooms_input = message.text.split()
        if len(rooms_input) != 2:
            raise ValueError("Введите два числа через пробел.")
        rooms_from, rooms_to = map(int, rooms_input)
        await state.update_data(rooms_from=rooms_from, rooms_to=rooms_to)
        await state.set_state(Form.finishing)
        keyboard = InlineKeyboardMarkup(
        inline_keyboard=[
            [InlineKeyboardButton(text="Да", callback_data="finishing_yes"),
             InlineKeyboardButton(text="Нет", callback_data="finishing_no")],
            [InlineKeyboardButton(text="Начать сначала", callback_data="start_over")]
        ]
        )
        await message.answer("Отделка: выберите 'Да' или 'Нет':", reply_markup=keyboard)
    except ValueError:
        await message.answer("Пожалуйста, введите корректный диапазон для количества комнат.")

@dp.callback_query(F.data.in_(["finishing_yes", "finishing_no"]))
async def process_finishing(callback: types.CallbackQuery, state: FSMContext):
    finishing = "да" if callback.data == "finishing_yes" else "нет"
    await state.update_data(finishing=finishing)
    await state.set_state(Form.construction_date)
    keyboard = InlineKeyboardMarkup(
        inline_keyboard=[
            [InlineKeyboardButton(text="Новостройка", callback_data="new_building"),
             InlineKeyboardButton(text="Втроичка", callback_data="not_new_building")],
            [InlineKeyboardButton(text="Начать сначала", callback_data="start_over")]
        ]
    )
    await callback.message.edit_text("Дата постройки: выберите 'Новостройка' или 'Вторичка':", reply_markup=keyboard)

@dp.callback_query(F.data.in_(["new_building", "not_new_building"]))
async def process_construction_date(callback: types.CallbackQuery, state: FSMContext):
    construction_date = "новостройка" if callback.data == "new_building" else "вторичка"
    await state.update_data(construction_date=construction_date)
    await state.set_state(Form.metro)
    await callback.message.edit_text("Введите название станции метро, возле которой вы хотите жить:",
                                     reply_markup=start_over_button())


@dp.message(Form.metro)
async def process_metro_station(message: types.Message, state: FSMContext):
    # Сохраняем введённую станцию метро
    metro_station = message.text.strip()
    await state.update_data(metro_station=metro_station)

    # Переходим к следующему шагу (максимальное время до метро)
    await state.set_state(Form.metro_time)
    await message.answer(
        "Введите максимальное время до метро (в минутах, например, 30):",
        reply_markup=start_over_button()
    )


@dp.message(Form.metro_time)
async def process_metro_time(message: types.Message, state: FSMContext):
    try:
        metro_time_to = int(message.text.strip())  # Максимальное время
        metro_time_from = 1  # Минимальное значение
        await state.update_data(metro_time_from=metro_time_from, metro_time_to=metro_time_to)
        await state.set_state(Form.park_time)
        await message.answer("Введите максимальное время до парка (в минутах):", reply_markup=start_over_button())
    except ValueError:
        await message.answer("Некорректный ввод. Пожалуйста, введите число.")

@dp.message(Form.park_time)
async def process_park_time(message: types.Message, state: FSMContext):
    try:
        park_time_to = int(message.text.strip())  # Максимальное время
        park_time_from = 1  # Минимальное значение
        await state.update_data(park_time_from=park_time_from, park_time_to=park_time_to)
        await state.set_state(Form.school_time)
        await message.answer("Введите максимальное время до школы (в минутах):", reply_markup=start_over_button())
    except ValueError:
        await message.answer("Некорректный ввод. Пожалуйста, введите число.")

@dp.message(Form.school_time)
async def process_school_time(message: types.Message, state: FSMContext):
    try:
        school_time_to = int(message.text.strip())  # Максимальное время
        school_time_from = 1  # Минимальное значение
        await state.update_data(school_time_from=school_time_from, school_time_to=school_time_to)
        await state.set_state(Form.clinic_time)
        await message.answer("Введите максимальное время до поликлиники (в минутах):", reply_markup=start_over_button())
    except ValueError:
        await message.answer("Некорректный ввод. Пожалуйста, введите число.")

@dp.message(Form.clinic_time)
async def process_clinic_time(message: types.Message, state: FSMContext):
    try:
        clinic_time_to = int(message.text.strip())  # Максимальное время
        clinic_time_from = 1  # Минимальное значение
        await state.update_data(clinic_time_from=clinic_time_from, clinic_time_to=clinic_time_to)
        await state.set_state(Form.kindergarten_time)
        await message.answer("Введите максимальное время до детского сада (в минутах):", reply_markup=start_over_button())
    except ValueError:
        await message.answer("Некорректный ввод. Пожалуйста, введите число.")

@dp.message(Form.kindergarten_time)
async def process_kindergarten_time(message: types.Message, state: FSMContext):
    try:
        kindergarten_time_to = int(message.text.strip())  # Максимальное время
        kindergarten_time_from = 1  # Минимальное значение
        await state.update_data(kindergarten_time_from=kindergarten_time_from, kindergarten_time_to=kindergarten_time_to)
        await state.set_state(Form.parking)
        keyboard = InlineKeyboardMarkup(
            inline_keyboard=[
                [InlineKeyboardButton(text="Да", callback_data="parking_yes"),
                 InlineKeyboardButton(text="Нет", callback_data="parking_no")],
                [InlineKeyboardButton(text="Начать сначала", callback_data="start_over")]
            ]
        )
        await message.answer("Нужен паркинг: выберите 'Да' или 'Нет':", reply_markup=keyboard)
    except ValueError:
        await message.answer("Некорректный ввод. Пожалуйста, введите число.")

@dp.callback_query(F.data.in_(["parking_yes", "parking_no"]))
async def process_parking(callback: types.CallbackQuery, state: FSMContext):
    parking = "да" if callback.data == "parking_yes" else "нет"
    await state.update_data(parking=parking)

    user_data = await state.get_data()
    model_input = {
        'sqft': f"{user_data.get('area_from')} {user_data.get('area_to')}",
        'floor': f"{user_data.get('floor_from')} {user_data.get('floor_to')}",
        'rooms': f"{user_data.get('rooms_from')} {user_data.get('rooms_to')}",
        'finishing ': f"{user_data.get('finishing')}",
        'type_of_housing': user_data.get('construction_date'),
        'subway': user_data.get('metro_station'),
        'time_to_subway': f"{user_data.get('metro_time_from')} {user_data.get('metro_time_to')}",
        'time_to_park': f"{user_data.get('park_time_from')} {user_data.get('park_time_to')}",
        'time_to school': f"{user_data.get('school_time_from')} {user_data.get('school_time_to')}",
        'time_to_hospital': f"{user_data.get('clinic_time_from')} {user_data.get('clinic_time_to')}",
        'time_to_kindergarten': f"{user_data.get('kindergarten_time_from')} {user_data.get('kindergarten_time_to')}",
        'parking_space': f"{user_data.get('parking')}"
    }
    u = ya_daun(model_input)
    await callback.message.edit_text(f"Средняя цена квартиры учитывая все Ваши запросы составляет: {u}")

    # Завершение: получение всех данных
    data = await state.get_data()
    result = "\n".join([f"{key.capitalize()}: {value}" for key, value in data.items()])
    await state.clear()

# Основная функция запуска
async def main():
    await dp.start_polling(bot)

if __name__ == "__main__":
    asyncio.run(main())
