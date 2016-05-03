import scrapy

class LineupSpider(scrapy.Spider):
    name = 'lineups'
    start_urls = ['http://www.baseballpress.com/lineups']

    def parse(self, response):
        for game in response.css('.game'):
            for player in game.css('.player_link'):
                yield {'player': player.xpath('text()').extract()}

    def parse_player_stats(self, response):
        for post_title in response.css('div.entries > ul > li a::text').extract():
            yield {'title': post_title}
