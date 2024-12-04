from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor
from sklearn.neighbors import KNeighborsRegressor
from sklearn.metrics import mean_squared_error, r2_score
from preprocessing import load_and_preprocess_data

def train_model(config):
    df = load_and_preprocess_data(config)
    X = df.drop(columns=['price']).values
    y = df['price'].values

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=config['test_size'], random_state=config['random_state'])

    # Обучение модели RandomForest
    rf = RandomForestRegressor(**config['model_params']['random_forest'])
    rf.fit(X_train, y_train)
    y_pred = rf.predict(X_test)

    # Метрики
    mse = mean_squared_error(y_test, y_pred)
    r2 = r2_score(y_test, y_pred)

    print(f'MSE: {mse}, R2: {r2}')
    return rf
