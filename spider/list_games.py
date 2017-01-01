#!/usr/bin/env python3

import sqlite3

import urllib

from urllib import request
from html.parser import HTMLParser

from multiprocessing import pool

class GameParser(HTMLParser):
  def __init__(self):
    super().__init__()
    self.game_urls = []

  def handle_starttag(self, tag, attrs):
    if tag != 'a':
      return
    classes = []
    href = None
    for attr, value in attrs:
      if attr == 'class':
        if not value:
          continue
        classes.extend(value.split(' '))
      elif attr == 'href':
        href = value

    if 'button-reviewMatch' in classes:
      self.game_urls.append(href)


  def handle_endtag(self, tag):
    pass

  def handle_data(self, data):
    pass

class UrlWriter:
  def __init__(self, conn):
    self.conn = conn
    self.urls = []

  def Flush(self):
    with self.conn:
      for url in self.urls:
        self.conn.execute('INSERT OR IGNORE INTO game_urls VALUES (?, "")', (url, ))
    self.urls = []

  def Insert(self, url):
    self.urls.append(url)
    if len(self.urls) >= 10:
      self.Flush()


def main():

  # Init database

  conn = sqlite3.connect('data.db')

  c = conn.cursor()
  c.execute('CREATE TABLE IF NOT EXISTS game_urls (url text, data text)')
  c.execute('CREATE UNIQUE INDEX IF NOT EXISTS  unique_url ON game_urls (url)')
  conn.commit()

  page = 1266
  urls = []
  writter = UrlWriter(conn)
  try:
    while True:
      print('Page {}'.format(page))
      url = 'http://theaigames.com/competitions/ultimate-tic-tac-toe/game-log/a/{}?_pjax=#page'.format(page)
      page += 1
      response = request.urlopen(url, timeout=15)
      data = response.read().decode('ascii')

      parser = GameParser()
      parser.feed(data)
      for url in parser.game_urls:
        print(url)
        writter.Insert(url)

  finally:
    writter.Flush()


if __name__ == '__main__':
  main()
