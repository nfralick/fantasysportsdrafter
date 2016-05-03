import scrapy

from player_stats.items import PlayerStats

class LineupsSpider(scrapy.Spider):
	name = "lineups"
	allowed_domains = ["baseballpress.com", "baseball-reference.com"]
	start_urls = ['http://www.baseballpress.com/lineups/2016-04-29']
	hcolumns = ["G","GS","PA","AB","R","H","2B","3B","HR","RBI","SB","CS","BB","SO","BA","OBP","SLG","OPS","TB","GDP","HBP","SH","SF","IBB","ROE","BAbip","tOPS+","sOPS+"]
	pcolumns = ["G","PA","AB","R","H","2B","3B","HR","SB","CS","BB","SO","SO/W","BA","OBP","SLG","OPS","TB","GDP","HBP","SH","SF","IBB","ROE","BAbip","tOPS+","sOPS+"]
	def parse(self, response):
		for game in response.css('.game'):
			teams = []
			players = 0
			for team_header in game.css('.team-data'):
				teams.append(team_header.css('.team-name').xpath('text()').extract()[0])
				player = PlayerStats()
				player['name'] = team_header.css('.player-link').xpath('text()').extract()[0]
				player['position'] = 'P'
				player['hand'] = team_header.xpath('div/div/text()').extract()[1][2]
				player['team'] = teams[-1]
				bref = team_header.css('.player-link').xpath('@data-bref').extract()[0]
				url = "http://www.baseball-reference.com/players/split.cgi?id=" + bref + "&year=2016&t=p"
				request = scrapy.Request(response.urljoin(url), self.parse_pitcher_stats)
			 	request.meta['item'] = player
				yield request

			for player_div in game.css('.players > div'):
				players += 1
				player = PlayerStats()
				player['name'] = player_div.xpath('a/text()').extract()[0]
				player['order'] = int(player_div.xpath('text()').extract()[0][0])
				player['position'] = player_div.xpath('text()').extract()[1][-2:].strip()
				player['hand'] = player_div.xpath('text()').extract()[1][2]
				if players < 11:
					player['team'] = teams[0]
					player['against'] = teams[1]
				else:
					player['team'] = teams[1]
					player['against'] = teams[0]
				bref = player_div.xpath('a/@data-bref').extract()[0]
				url = "http://www.baseball-reference.com/players/split.cgi?id=" + bref + "&year=2016&t=b"
				request = scrapy.Request(response.urljoin(url), self.parse_hitter_stats)
				request.meta['item'] = player
				yield request

	def parse_hitter_stats(self, response):
		player = response.meta['item']
		for row in response.css('#plato').xpath('tbody/tr'):
			row_type = ''
			cell_num = 0
			for cell in row.css('td'):
				if cell_num == 0:
					if cell.xpath('span/text()').extract()[0].find("vs LH Starter") != -1:
						row_type = 'vLHS'
						player['vLHS'] = {}
					if cell.xpath('span/text()').extract()[0].find("vs RH Starter") != -1:
						row_type = 'vRHS'
						player['vRHS'] = {}
				else:
					if row_type == '':
						break
					player[row_type][self.hcolumns[cell_num-1]] = float(cell.xpath('text()').extract()[0])
				cell_num += 1

		yield player

	def parse_pitcher_stats(self, response):
		player = response.meta['item']
		for row in response.css('#plato').xpath('tbody/tr'):
			row_type = ''
			cell_num = 0
			for cell in row.css('td'):
				if cell_num == 0:
					if cell.xpath('span/text()').extract()[0].find("vs RHB") != -1:
						row_type = 'vLHB'
						player['vLHB'] = {}
					if cell.xpath('span/text()').extract()[0].find("vs LHB") != -1:
						row_type = 'vRHB'
						player['vRHB'] = {}
				else:
					if row_type == '':
						break
					if cell.xpath('text()').extract():
						player[row_type][self.pcolumns[cell_num-1]] = float(cell.xpath('text()').extract()[0])
				cell_num += 1

		yield player
