import pandas as pd
from sklearn.preprocessing import OneHotEncoder, LabelEncoder, StandardScaler
import yaml

# Загрузка конфигурации
with open("config.yml", "r") as file:
    config = yaml.safe_load(file)

def load_data(path):
    return pd.read_csv(path)

def preprocess_data(df):
    # Удаление ненужных столбцов
    df.drop(columns=config["preprocessing"]["drop_columns"], inplace=True)

    # Преобразование категориальных данных
    categorical_columns = config["preprocessing"]["categorical_columns"]
    unprocessed_cat_features = df[categorical_columns].select_dtypes(include=[object]).columns.tolist()

    encoder = OneHotEncoder(sparse_output=False)
    one_hot_encoded = encoder.fit_transform(df[unprocessed_cat_features])
    one_hot_df = pd.DataFrame(one_hot_encoded, columns=encoder.get_feature_names_out(unprocessed_cat_features), index=df.index)

    df = pd.concat([df, one_hot_df], axis=1).drop(unprocessed_cat_features, axis=1)

    df.loc[(df["rooms"] == "Open layout"), ["rooms"]] = "0"
    df[["rooms", "price"]] = df[["rooms", "price"]].apply(pd.to_numeric)

    # Масштабирование данных для knn
    #scaler = StandardScaler()
    #numerical_columns = config["preprocessing"]["drop_columns"]
    #df[numerical_columns] = scaler.fit_transform(df[numerical_columns])

    return df

# Использование
data = load_data(config["data"]["path"])
processed_data = preprocess_data(data)
