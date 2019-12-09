from flask import Flask, render_template
import requests
import json
import os
from selenium import webdriver

app = Flask(__name__)

arr = []

@app.route('/table')
def table():
	return render_template('index.html',arr=arr)

@app.route('/')
def init():
	driver = webdriver.Firefox()
	driver.minimize_window()
	x = driver.get("http://onem2m.iiit.ac.in:443/webpage")
	x = driver.find_element_by_xpath("/html/body/div[1]/center/table/tbody/tr[3]/td[2]/button")
	x.click()
	x = driver.find_element_by_xpath("/html/body/div[2]/div[2]/div[1]/div/ul/li/ul/li[9]")
	x.click()
	x = driver.find_element_by_xpath("/html/body/div[2]/div[2]/div[1]/div/ul/li/ul/li[9]/ul/li[3]")
	x.click()
	i = 0

	f = open("in.txt","w")

	while [1]:
		i += 1
		os.system('clear')
		print(i,"Names Downloaded.....Please Wait")
		try:
			x = driver.find_element_by_xpath("/html/body/div[2]/div[2]/div[1]/div/ul/li/ul/li[9]/ul/li[3]/ul/li[" + str(i) + "]")
		except:
			break
		f.write(x.text + "\n")

	driver.close()
	f.close()

	f = open("./in.txt","r")
	inp = f.read().split("\n")
	f.close()

	cnt = 0
	for i in inp:
		cnt += 1
		headers = {"X-M2M-Origin": "admin","Content-Type":"application/json"}
		if i == "undefined" or i == "":
			continue
		lol = "http://onem2m.iiit.ac.in:443/~/in-cse/" + i.replace("_","-");
		x = requests.get(lol, headers=headers)
		x = x.json()
		
		if x["m2m:cin"]["con"] == "test_instance":
			continue
		arr.append([int(x["m2m:cin"]["ct"].replace("T","")),float(x["m2m:cin"]["con"])])
		os.system('clear')
		print(str((cnt/len(inp))*100) + "% Downloaded")
	return render_template('ready.html')

@app.route('/graph')
def graph():
	return render_template('graph.html',arr=arr)

if __name__ == '__main__':
  app.run(debug=True)