

#SYN:xflajs00
"""
Predmet: IPP
Projekt: proj2
Varianta zadani: SYN (zvyrazneni syntaxe)
@author: Petr Flajsingr, xflajs00
"""

#knihovny
import sys
import re
import codecs
import getopt

#navratove kody skriptu:
#0 - vse probehlo v poradku
#1 - chyba argumentu
#2 - chyba vstupniho souboru
#3 - chyba vystupniho souboru
#4 - chyba obsahu formatovaciho souboru

re.DOTALL #tecka je i enter
#vypis napovedy, ukoncuje skript
def printHelp():
	print("Skript pro automaticke zvyraznovani textu podle formatovaciho souboru\n\n\
Pouziti skriptu:\n\n\
--help\t\t\t-\tVypis napovedy\n\
--format=filename\t-\tUrceni formatovaciho souboru\n\
--input=filename\t-\tUrceni vstupniho souboru\n\
--output=filename\t-\tUrceni vystupniho souboru\n\
--br\t\t\t-\tPridani <br /> na konec kazdeho radku vystupniho souboru")
	exit(0)

#funkce pro vypis na stderr a ukonceni skriptu
#@param str
#str pro vypis chyby
#@param code
#kod pro ukonceni skriptu
def printError(str, code):
	sys.stderr.write(str + '\n')
	exit(code)

#kontrola a ulozeni argumentu
#@param args
#vstupni argumenty skriptu
#@return params
#zpracovane argumenty
def getParams(args):
	try:
		opts, args = getopt.getopt(args[1:], "h", ["help", "output=", "input=", "format=", "br"])
	except:
		printError('Chyba argumentu', 1)
	#prostor pro ulozeni informaci argumentu
	params = {'inFile':'', 'inGiven':0, 'outFile':'','outGiven':0, 'formFile':'', 'formGiven':0, 'br':0}
	#pruchod vsemi argumenty
	for o, a in opts: 
		if(o == "-h" or o == "--help"):
			printHelp()
		elif(o == "--input"):
			if(params['inGiven'] == 1):
				printError('Chyba argumentu', 1)
			params['inGiven'] = 1
			params['inFile'] = a
		elif(o == "--output"):
			if(params['outGiven'] == 1):
				printError('Chyba argumentu', 1)
			params['outGiven'] = 1
			params['outFile'] = a
		elif(o == "--format"):
			if(params['formGiven'] == 1):
				printError('Chyba argumentu', 1)
			params['formGiven'] = 1
			params['formFile'] = a
		elif(o == "--br"):
			if(params['br'] == 1):
				printError('Chyba argumentu', 1)
			params['br'] = 1
		else: #neznamy argument
			printError('Chyba argumentu', 1)
	return params

#otevreni souboru
#@param fileName
#cesta/nazev souboru
#@return file
#obsah souboru
def openFile(fileName, sw):
	try:
		temp = codecs.open(fileName, "r", "utf-8")
	except:
		if(sw):
			printError('Chyba otevreni souboru: \"'+fileName+'\"', 2)
		else:
			return 0
	if(sw):
		#input
		file = temp.read()
	else:
		#format
		file = temp.readlines()
		for i in range(len(file)):
			file[i] = re.sub("\n", "", file[i])
	temp.close()
	return file

#vypis do souboru
#@param fileName
#cesta/nazev souboru k vypisu
#@param br
#prepinac pro pridani <br />
#@param output
#vystupni data
def printOut(outGiven, fileName, br, output):
	if(br):
		output = re.sub('\n', '<br />\n', output)
	#vypis do souboru
	if(outGiven):
		try:
			file = codecs.open(fileName, "w", "utf-8")
		except:
			printError('Chyba otevreni souboru: \"'+fileName+'\"', 3)
		file.write(output)
		file.close()
	#vypis na stdout
	else:
		sys.stdout.write(output)

#nahrazeni vyrazu
#@param str
#vstupni vyraz
#@return str
#zmeneny vyraz
def regSub(str):
	#uprava problemovych znaku
	str = re.sub('\[', '\\[', str)
	str = re.sub('\]', '\\]', str)
	str = re.sub('\{', '\\{', str)
	str = re.sub('\}', '\\}', str)
	str = re.sub('\?', '\\?', str)
	str = re.sub('\^', '\\^', str)
	str = re.sub('\$', '\\\$', str)
	#chybne zadany vyraz
	if(re.search('!(?=[*+!|()])', str) != None):
		return 0
	#uprava "negovanych" vyrazu
	str = re.sub('!%s', r'\S', str)
	str = re.sub('!%a', r'[^.]', str)
	str = re.sub('!%d', r'[^0-9]', str)
	str = re.sub('!%l', r'[^a-z]', str)
	str = re.sub('!%L', r'[^A-Z]', str)
	str = re.sub('!%w', r'[^a-zA-Z]', str)
	str = re.sub('!%W', r'[^\w]', str)
	str = re.sub('!%t', r'[^\t]', str)
	str = re.sub('!%n', r'[^\n]', str)
	str = re.sub('!(.)', r'[^\1]', str)
	#uprava normalnich vyrazu
	str = re.sub('%s', '\r\n|\n|\t|\f|\v| ', str)
	str = re.sub('%a', '(.|\n)', str)
	str = re.sub('%d', '\\d', str)
	str = re.sub('%l', '[a-z]', str)
	str = re.sub('%L', '[A-Z]', str)
	str = re.sub('%w', '[a-zA-Z]', str)
	str = re.sub('%W', '\w', str)
	str = re.sub('%t', '\\t', str)
	str = str.replace('\\t', '\\\\t') 
	str = re.sub('%n', '\\n', str)
	str = str.replace('\\n', '\\\\n')
	str = re.sub('%[.]', '\\.', str)
	str = re.sub('%[|]', '\\|', str)
	str = re.sub('%[!]', '!', str)
	str = re.sub('%[*]', '\\*', str)
	str = re.sub('%[+]', '\\+', str)
	str = re.sub('%[(]', '\\(', str)
	str = re.sub('%[)]', '\\)', str)
	str = re.sub('%%', '%', str)
	return str

#uprava regulernich vyrazu
#@param str
#regulerni vyraz z formatovaciho souboru
#@return 
#upraveny vyraz
def getReg(str):
	#odstraneni pripadnych tecek
	temp2 = -1
	for i in re.finditer('\.', str):
		if(i.start() != i.end() and i.start() > 0 and str[i.start() - 1] != '\\' and str[i.start() - 1] != '%'):
			if(temp2 == i.start() - 1):#vice tecek za
				temp.insert(temp2, '.')
				printError('Chyba v regulernim vyrazu: "' + "".join(temp) + '\"', 4)
			temp = list(str)
			del(temp[i.start()])
			str = "".join(temp)
			temp2 = i.start()
	str = regSub(str) #uprava pomoci re.sub
	try:
		re.compile(str)
	except:
		printError('Chyba v regulernim vyrazu: "' + str + '\"', 4)
	return str

#Vytvoreni tagu na zaklade formatovacich prikazu
#@param temp
#pole formatovacich prikazu
#@return fin
#pole prikazu prevedenych na HTML tagy
def changeTag(temp):
	start = ""
	end = ""
	for i in range(len(temp)):
		if(temp[i] == "bold"):
			start += "<b>"
			end = "</b>" + end
		elif(temp[i] == "italic"):
			start += "<i>"
			end = "</i>" + end
		elif(temp[i] == "underline"):
			start += "<u>"
			end = "</u>" + end
		elif(temp[i] == "teletype"):
			start += "<tt>"
			end = "</tt>" + end
		elif((re.match(r'size:([1-7])', temp[i])) != None):
			result = re.match(r'size:([1-7])', temp[i])
			start += "<font size=" + result.group(1)+">"
			end = "</font>"+end
		elif((re.match(r'color:([0-9a-fA-F]{6})', temp[i])) != None):
			result = re.match(r'color:([0-9a-fA-F]{6})', temp[i])
			start += "<font color=#" + result.group(1)+">"
			end = "</font>"+end
		elif(temp[i] == ""):
			continue
		else:
			printError('Chyba v tagu: "' + "".join(temp) + '\"', 4)
	fin = []
	fin.append(start)
	fin.append(end)

	return fin

#extrakce tagu
#@param str
#prikazy
#@return temp
#prevedene prikazy na HTML tagy
def getTag(str):
	#odstraneni bilych znaku
	str = re.sub(r'\s', '', str)
	temp = str.split(",")
	temp = changeTag(temp)
	return temp

#zpracovani dat z formatovaciho souboru
#@param form
#formatovaci soubor
#@return info
#zpracovana data
def formatFile(form):
	temp = []
	info = [[]]
	for i in range(len(form)):
		temp = form[i].partition("\t")
		if(temp[1] != "\t"):
			printError('Chyba ve formatovacim souboru', 4)
		temp2 = getReg(temp[0])
		info[i].append(temp2)
		temp2 = getTag(temp[2])
		info[i].append(temp2)
		info[i].append(len(temp2[0]))
		info[i].append(len(temp2[1]))
		if(i < (len(form) - 1)):
			info.append([]) #zvetseni velikosti seznamu
	return info

#nalezeni stringu pro oznaceni tagem
#@param info
#data obsahujici informace o vyhledavanych
#@param input
#data, ve kterych se vyhledava
def findPos(info, input):
	for i in range(len(info)): #pruchod vsemi stringy
		if(info[i] != []): #neprazdny list
			#vytvoreni novych polozek
			info[i].append([])
			info[i].append([])
			for j in re.finditer(info[i][0], input): #samotne vyhledavani
				if(j.start() != j.end()): #pokud byl nalezen
					info[i][4].append(j.start()) #ulozeni pocatecnich hodnot
					info[i][5].append(j.end()) #ulozeni konecnych hodnot
	return info

#posun indexu po vlozeni pocatecniho tagu
#@param info
#informace o pozicich
#@param i, j, k, l
#promenne urcujici menene pozice
def moveIndexS(info, i, j, k, l):
	if (info[i][4][j] < info[k][4][l] or (info[i][4][j] == info[k][4][l] and i != k)):
		info[k][4][l] += info[i][2]
	if (info[i][4][j] < info[k][5][l]):
		info[k][5][l] += info[i][2]

#posun indexu po vlozeni ukoncujiciho tagu
#@param info
#informace o pozicich
#@param i, j, k, l
#promenne urcujici menene pozice
def moveIndexE(info, i, j, k, l):
	if (info[i][5][j] < info[k][4][l] or (info[i][5][j] == info[k][4][l])):
		info[k][4][l] += info[i][3]
	if (info[i][5][j] < info[k][5][l]):
		info[k][5][l] += info[i][3]

#pridani tagu do vstupu
#@param info
#informace o: tag, pozice vyhledavaneho retezce
#@param input
#vstupni data
#@return input
#zmenena data
def addTags(info, input):
	temp = ""
	for i in range(len(info)): #kontrola podle vsech zaznamu
		if (info[i] != []):
			for j in range(len(info[i][4])): #prochazeni jednotlivych pozic
				#pridani zacatecniho tagu
				temp = input[info[i][4][j]:]
				input = input[:info[i][4][j]] + info[i][1][0]
				input = input[:len(input)] + temp
				#posun pozic o pridane tagy
				for k in range(len(info)):
					for l in range(len(info[k][4])):
						moveIndexS(info, i, j, k, l)
				#pridani ukoncujiciho tagu
				temp = input[info[i][5][j]:]
				input = input[:info[i][5][j]] + info[i][1][1]
				input = input[:len(input)] + temp
				#posun pozic o pridane tagy
				for k in range(len(info)):
					for l in range(len(info[k][4])):
						moveIndexE(info, i, j, k, l)
	return input



#Hlavni telo skriptu
#ukonci skript s kodem 0 pri uspesne exekuci
args = sys.argv
params = getParams(args)
#ziskani vstupnich dat
if(params['inGiven']):
	input = openFile(params['inFile'], 1)
else:
	input = sys.stdin.read() #cteni ze stdin bez zadani vstupniho souboru
#formatovaci soubor
if(params['formGiven']):
	form = openFile(params['formFile'], 0)
	#prazdny nebo neplatny formatovaci soubor
	if(form != 0 and form != ""):
		info = formatFile(form)
		if(info != 0): #pokud je formatovaci soubor v poradku
			#nalezeni poloh
			info = findPos(info, input)
			#vytisknuti a posun
			input = addTags(info, input)
printOut(params['outGiven'], params['outFile'], params['br'], input)
exit(0)