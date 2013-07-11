import sys
import numpy
from utils import *

class LogisticRegression(object):
    """description of class"""
    def __init__(self, input, label, n_in, n_out):
        self.x = input
        self.y = label
        self.W = numpy.zeros((n_in, n_out)) #initialize W 0
        self.b = numpy.zeros(n_out) #initialize bias 0

    def train(self, learning_rate=0.1, input=None, L2_reg=0.00):
        if input is not None:
            self.x = input
        
            p_y_given_x = softmax(numpy.dot(self.x, self.W) + self.b)
            d_y = self.y - p_y_given_x
            grad = numpy.dot(self.x.T, d_y)
            
            self.W += learning_rate * grad - learning_rate * L2_reg * self.W
            self.b += learning_rate * numpy.mean(d_y, axis=0)

    def loss_function():
        loss = -numpy.mean()

    def negative_log_likelihood(self):
        #loss function
        """
        Return the mean of the negtive log-likelihood of prediction 
        of the model under a given target distribution

        y: corresponds to a vector that gives for each example the correct label

        NOTE: use mean instead of sum so that 
              the learning rate is less dependent on the batch size
        """
        p_y_given_x = softmax(numpy.dot(self.x, self.W) + self.b) #sigmoid_activation
        
        cross_entropy = -numpy.mean(numpy.sum(self.y * numpy.log(p_y_given_x) +
                                              (1-self.y) * numpy.log(1-p_y_given_x), axis=1))
        return cross_entropy

    def predict(self, x):
        return softmax(numpy.dot(x, self.W) + self.b)


def sgd_optimization(learning_rate=0.13, n_epochs=1000, 
                     dataset='../data/mnist.pkl.gz', batch_size=600):
    classifier = LogistiRegression()
    cost = classifier.negative_log_likelihood(y)

    #early-stopping para

    done_looping = False

    for epoch in range(n_epochs):
        #update learning rate
        pass

        
        


