import sys
import numpy

class HiddenLayer(object):
    """description of class"""
    def __init__(self, input, n_in, n_out,
                 W=None, b=None, numpy_rng=None, activation=numpy.tanh):        
        if numpy_rng is None:
            numpy_rng = numpy.random.RandomState(1234)

        if W is None:
            a = 1. / n_in
            initial_W = numpy.array(numpy_rng.uniform(  # initialize W uniformly
                low=-a,
                high=a,
                size=(n_in, n_out)))

            W = initial_W

        if b is None:
            b = numpy.zeros(n_out)  # initialize bias 0


        self.numpy_rng = numpy_rng
        self.input = input
        self.W = W
        self.b = b

        self.activation = activation

        # self.params = [self.W, self.b]

    def output(self, input=None):
        pass

    def sample_h_given_v(self, input=None):
        pass


