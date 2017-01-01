#!/usr/bin/env python3

import sqlite3
import socket
import urllib

from urllib import request
from html.parser import HTMLParser

from multiprocessing import pool

def RetrieveGameData(url):
  print('Retrieving {}'.format(url))
  try:
    response = request.urlopen(url + '/data', timeout=15)
    data = response.read().decode('ascii')
  except socket.timeout:
    print('Timeout')
    return url, ''
  except urllib.error.URLError:
    print('Timeout')
    return url, ''
  code = response.getcode()
  if code != 200:
    print('Got code {}'.format(code))
    return url, ''
  if not data:
    data = 'gone'
  return url, data

class GameWriter:
  def __init__(self, conn):
    self.conn = conn
    self.data = []

  def Flush(self):
    with self.conn:
      for url, data in self.data:
        self.conn.execute('UPDATE game_urls SET data = ? where url = ?', (data, url))
    self.data = []

  def Insert(self, url, data):
    self.data.append((url, data))
    if len(self.data) >= 10:
      self.Flush()

def main():

  # Init database

  conn = sqlite3.connect('data.db')

  c = conn.cursor()
  c.execute('CREATE TABLE IF NOT EXISTS game_urls (url text, data text)')
  c.execute('CREATE UNIQUE INDEX IF NOT EXISTS  unique_url ON game_urls (url)')
  conn.commit()

  p = pool.ThreadPool(8)

  urls = sorted(url for (url,) in conn.execute('SELECT url FROM game_urls WHERE data = ""'))
  print('{} games to fetch'.format(len(urls)))


  writer = GameWriter(conn)
  try:
    for url, game_data in p.imap_unordered(RetrieveGameData, urls):
      if not game_data:
        print('Failed to update {}'.format(url))
        continue
      writer.Insert(url, game_data)
  finally:
    writer.Flush()


if __name__ == '__main__':
  main()
