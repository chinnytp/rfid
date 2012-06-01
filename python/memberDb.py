import sqlite3

connection = sqlite3.connect('test.db')

c = connection.cursor()

class MemberDb:
	def __init__(self):
		c.execute('''CREATE TABLE IF NOT EXISTS cards
             		(cardId text, disabled integer)''')
		connection.commit()

	def checkCard(self, tagId):
		p = (tagId, )
		c.execute("SELECT * FROM cards WHERE lower(cardId)=lower(?) AND disabled=0", p)
		return c.fetchone() != None

