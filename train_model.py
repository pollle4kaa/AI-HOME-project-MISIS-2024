from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error, r2_score
import pickle
import yaml
import pandas as pd

# Загрузка конфигурации
with open("config.yml", "r") as file:
    config = yaml.safe_load(file)

def train_model(data, target_column):
    X = data.drop(columns=[target_column])
    y = data[target_column]

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=config["model"]["random_forest"]["random_state"])

    # Обучение модели
    model = RandomForestRegressor(
        n_estimators=config["model"]["random_forest"]["n_estimators"],
        max_depth=config["model"]["random_forest"]["max_depth"],
        random_state=config["model"]["random_forest"]["random_state"]
    )
    model.fit(X_train, y_train)

    # Оценка модели
    y_pred = model.predict(X_test)
    mse = mean_squared_error(y_test, y_pred)
    r2 = r2_score(y_test, y_pred)

    print(f"MSE: {mse}")
    print(f"R2: {r2}")

    # Сохранение модели
    with open("pipeline/finalized_model.sav", "wb") as file:
        pickle.dump(model, file)

    return model

# Использование
df = pd.read_csv(config["data"]["path"])
trained_model = train_model(df, target_column="price")
