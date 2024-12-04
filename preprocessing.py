import pandas as pd
from sklearn.preprocessing import OneHotEncoder, LabelEncoder

def load_and_preprocess_data(config):
    df = pd.read_csv(config['data_path'])

    # Удаляем ненужные колонки
    df.drop(columns=['max_floor', 'transport_accessibility', 'park', 'reservoirs',
                     'mall', 'shool', 'hospital', 'kindergarten', 'link'], inplace=True)

    # Преобразование категориальных данных
    categorical_columns = ['finishing', 'type_of_housing', 'subway', 'parking_space']
    encoder = OneHotEncoder(sparse_output=False)
    one_hot_encoded = encoder.fit_transform(df[categorical_columns])
    one_hot_df = pd.DataFrame(one_hot_encoded, columns=encoder.get_feature_names_out(categorical_columns), index=df.index)
    df = pd.concat([df, one_hot_df], axis=1).drop(columns=categorical_columns)

    # Преобразование числовых колонок
    df[['rooms', 'price']] = df[['rooms', 'price']].apply(pd.to_numeric)

    return df
