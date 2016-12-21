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


  def __len__(self):
    return len(self._features)

def infinity():
  i = 0
  while True:
    yield i
    i += 1

class Classifier:
  def __init__(self, features_count, rotations_count):
    self.features = tf.placeholder(tf.float32, [None, rotations_count, features_count], name='features')
    self.y_true = tf.placeholder(tf.float32, [None], name='y_true')

    self.W = tf.Variable(tf.zeros([features_count]))
    self.b = tf.Variable(tf.zeros([1]))

    feat_reshaped = tf.reshape(self.features, [-1, features_count])
    accum = (tf.reduce_sum(feat_reshaped * self.W, 1) + self.b)
    accum = tf.reshape(accum, [-1, rotations_count])
    accum = tf.reduce_mean(accum, 1)
    self.y = tf.sigmoid(accum)

    reg = tf.reduce_mean(tf.square(self.W))

    # Minimize the mean squared errors.
    self.error = tf.reduce_mean(tf.abs(self.y - self.y_true))
    self.loss = self.error * 100 + reg
    optimizer = tf.train.AdamOptimizer(0.001)
    self.train = optimizer.minimize(self.loss)

    init = tf.global_variables_initializer()

    self.sess = tf.Session()
    self.sess.run(init)

  def fit(self, features, y_true):
    data_set = DataSet(features, y_true)

    BATCH_SIZE = 400
    try:
      last_improve = 0
      best_loss = 1.0e10
      CHECK = 1000
      for step in infinity():
        batchx, batchy = data_set.Next(BATCH_SIZE)
        feed = {self.features:batchx, self.y_true:batchy}
        self.sess.run(self.train, feed_dict=feed)
        if step % CHECK == 0 and step > 0:
          epoch = step // CHECK
          feed = {self.features:data_set._features, self.y_true:data_set._labels}
          score = self.sess.run(self.loss, feed_dict=feed)
          print(step, score)
          if score < best_loss:
            best_loss = score
            last_improve = epoch
          elif epoch - last_improve > 5:
            # Stop if it didn't improve for a while
            break
    except KeyboardInterrupt:
      pass

  
  @property
  def coef_(self):
    return self.sess.run(self.W)

  @property
  def intercept_(self):
    return self.sess.run(self.b)

  def predict_proba(self,features):
    feed = {self.features:features}
    return self.sess.run(self.y, feed_dict=feed)

  def score(self, features, y_true):
    feed = {self.features:features, self.y_true:y_true}
    return 1.0 - self.sess.run(self.error, feed_dict=feed)

