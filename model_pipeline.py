import pickle

def save_model(model, filename='finalized_model.sav'):
    with open(filename, 'wb') as f:
        pickle.dump(model, f)

def load_model(filename='finalized_model.sav'):
    with open(filename, 'rb') as f:
        return pickle.load(f)
