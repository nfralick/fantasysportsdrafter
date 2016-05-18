# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# http://doc.scrapy.org/en/latest/topics/items.html

import scrapy

class PlayerStats(scrapy.Item):
	name = scrapy.Field()
	order = scrapy.Field()
	position = scrapy.Field()
	hand = scrapy.Field()
	vLHS = scrapy.Field()
	vRHS = scrapy.Field()
	vLHB = scrapy.Field()
	vRHB = scrapy.Field()
	team = scrapy.Field()
	against = scrapy.Field()
	gamelog = scrapy.Field()
	pass
