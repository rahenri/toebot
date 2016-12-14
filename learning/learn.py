import tensorflow as tf
import numpy as np

class DataSet:
  def __init__(self, features, labels):
    self._features = features
    self._labels = labels
    self._offset = 0
    self._examples = features.shape[0]
    self._indices = np.arange(self._examples)
    self.epochs = 0

  def Next(self, batch_size):
    if self._offset + batch_size >= self._examples:
      self._offset = 0
      np.random.shuffle(self._indices)
      self._features = self._features[self._indices]
      self._labels = self._labels[self._indices]
      self.epochs += 1

    start = self._offset
    end = self._offset + batch_size

    self._offset += batch_size

    return self._features[start:end], self._labels[start:end]

class Classifier:
  def __init__(self, features_count):
    self.features = tf.placeholder(tf.float32, [None, features_count])
    self.y_true = tf.placeholder(tf.float32, [None])

    self.W = tf.Variable(tf.random_uniform([features_count], -1.0, 1.0))
    self.b = tf.Variable(tf.zeros([1]))
    self.y = tf.sigmoid(tf.reduce_sum(self.features * self.W, 1) + self.b)
    reg = tf.reduce_mean(tf.square(self.W)) * 1.0

    # Minimize the mean squared errors.
    self.error = tf.reduce_mean(tf.square(self.y - self.y_true))
    self.loss = self.error + reg
    optimizer = tf.train.AdamOptimizer(0.01)
    self.train = optimizer.minimize(self.loss)

    init = tf.global_variables_initializer()

    self.sess = tf.Session()
    self.sess.run(init)

  def fit(self, features, y_true):
    data_set = DataSet(features, y_true)

    try:
      for step in range(100000):
        batchx, batchy = data_set.Next(100)
        feed = {self.features:batchx, self.y_true:batchy}
        self.sess.run(self.train, feed_dict=feed)
        if step % 1000 == 0:
          feed = {self.features:data_set._features, self.y_true:data_set._labels}
          print(step, self.sess.run(self.loss, feed_dict=feed))
    except KeyboardInterrupt:
      pass

  def coef(self):
    return self.sess.run(self.W)

  def bias(self):
    return self.sess.run(self.b)

  def predict_proba(self,features):
    feed = {self.features:features}
    return self.sess.run(self.y, feed_dict=feed)

  def score(self, features, y_true):
    feed = {self.features:features, self.y_true:y_true}
    return 1.0 - self.sess.run(self.error, feed_dict=feed)

