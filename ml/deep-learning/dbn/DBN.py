class DBN(object):
    """
    This class implements a deep belief network
    """
    def __init__(self, model):
        self.models = []
        self.models.append(model)

    def forward(self, X):
        for model in self.models:
            X = model.forward(X)

        return X

    def backward(self, Y):
        for model in reversed(self.models):
            Y = model.backward(Y)

        return Y

    def sample(self, num_samples=1, burn_in_length=100, sample_spacing=20, num_parallel_chains=1):
        """
        Draws samples from the model
        """

        #sample from the top model
        Y = self.models[-1].sample(num_samples, burn_in_length, sample_spacing, num_parallel_chains)

        #propagate samples down to the visible units
        for model in reversed(self.models[:-1]):
            Y = model.backward(Y)
        return Y

    def estimate_log_partition_function(self, num_ais_samples=100, beta_weights=[], layer=-1):


