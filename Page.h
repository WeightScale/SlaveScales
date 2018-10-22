﻿// Page.h

#ifndef _PAGE_h
#define _PAGE_h

const char netIndex[] PROGMEM = R"(<html lang='en'><meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/><body><form method='POST'><input name='ssid'><br/><input type='password' name='key'><br/><input type='submit' value='СОХРАНИТЬ'></form></body></html>)";

const char settings_html[] PROGMEM = R"(<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate'/> <meta http-equiv='Pragma' content='no-cache'/> <title>Настройки</title> <link rel='stylesheet' type="text/css" href='global.css'> <style>input:focus{background: #FA6; outline: none;}table{width: 100%;}input{width: 100%; text-align: right; font-size: 18px;}input[type=submit]{width: auto;}</style> <script>var d=document; function GetValue(){let r=new XMLHttpRequest(); r.overrideMimeType('application/json'); r.onreadystatechange=function(){if (r.readyState===4){if (r.status===200){let j=JSON.parse(r.responseText); for (v in j){try{d.getElementById(v).innerHTML=j[v];}catch (e){}}}}}; r.open("GET", "/sv", true); r.send(null);}function GetSettings(){let r=new XMLHttpRequest(); r.overrideMimeType('application/json'); r.onreadystatechange=function(){if (r.readyState===4){if (r.status===200){try{let j=JSON.parse(r.responseText); for (v in j){try{d.getElementById(v).value=j[v];}catch (e){}}}catch (e){alert(e.toString());}finally{d.body.style.visibility='visible'; GetValue();}}}}; r.open('GET', '/settings.json', true); r.send(null);}function formNet(i){let f=new FormData(d.getElementById(i)); let r=new XMLHttpRequest(); r.onreadystatechange=function(){if (r.readyState===4){if (r.status===200){let rec=confirm('Перегрузить'); if (rec){r.onreadystatechange=null; r.open('GET', '/rc', true); r.send(null);}}else if (r.status===400){alert('Настройки не изменились');}}}; r.open('POST', '/settings.html', true); r.send(f);}window.onload=function(){GetSettings();}; </script></head><body style="visibility: hidden"><strong>Настройки</strong><hr><br/><fieldset style="width: auto"> <details> <summary>Общии настройки</summary> <br><h5 align='left'><b>для соединения с MASTER</b></h5> <form id='form_ssid_id' action='javascript:formNet("form_ssid_id")'> <table> <tr> <td>MASTER:</td><td> <input id='id_ssid' name='ssid' placeholder='имя master'> </td></tr><tr> <td>SLAVE: </td><td> <input id='id_assid' name='assid' placeholder='имя slave'> </td></tr><tr> <td></td><td> <input type='submit' value='СОХРАНИТЬ'/> </td></tr></table> </form> <hr> <form method='POST'> <h5>Доступ к настройкам</h5> <table> <tr> <td>ИМЯ:</td><td> <input id="id_n_admin" name="n_admin" placeholder='имя админ'> </td></tr><tr> <td>ПАРОЛЬ:</td><td> <input type="password" id="id_p_admin" name="p_admin" placeholder='пароль админ'> </td></tr><tr> <td></td><td> <input type='submit' value='СОХРАНИТЬ'/> </td></tr></table> </form> </details></fieldset><br/><fieldset> <details> <summary>Информация</summary> <br><table> <tr> <td> <h5>Имя хоста:</h5></td><td align='right'> <h5 id='id_local_host'></h5></td></tr></table> <hr> <h5 align='left'><b>Точка доступа весов</b></h5> <table> <tr> <td id='id_ap_ssid'></td><td align='right' id='id_ap_ip'></td></tr></table> <hr> <table> <tr> <td>Пломба:</td><td align='right'> <div id='sl_id'></div></td></tr></table> <hr> <a href='/calibr.html'>калибровка</a> </details></fieldset><hr><footer align='center'>2018 © Powered by www.scale.in.ua</footer></body></html>)";

const char global_css[] PROGMEM = R"(html{background: #ffffff;}body{font-family: Arial, sans-serif;width: 330px;height: auto;margin-right: auto;margin-left: auto;border: 5px solid #d3d3d3;background: #d3d3d3;}h1{margin: inherit;}h2{margin: auto;}h4,h5{margin: auto;}fieldset{background: #c1c1c1;border: 0px;}#brand_name{color: white;text-align: right;}img{width:auto;height:30px;}footer{font-family: Arial, sans-serif;font-size: small;}.btn, .btn:link, .btn:visited {display: inline-block;text-decoration: none;text-transform: uppercase;}.btn:hover, .btn:focus {color: white;}.btn--s {padding: 0.5em;}.btn--m {padding: 0.5em;width: 100%;padding-left: 0px;padding-right: 0px;}.btn--full, .btn--full:link {display: block;margin-left: auto;margin-right: auto;text-align: center;width: 100%;}.btn--blue:link, .btn--blue:visited {background-color: #c1c1c1;}.btn--blue:hover, .btn--blue:focus {background-color: #a0a0a0;})";

const char calibr_html[] PROGMEM = R"(<!DOCTYPE html>
<html lang='en'>

<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no' />
<meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />
<meta http-equiv='Pragma' content='no-cache' />
<title>Калибровка</title>
<link rel='shortcut icon' href='favicon.png' type='image/png'>
<link rel='stylesheet' type='text/css' href='global.css'>
<style>
#wt_id {
	display: inline;
	background: #fff;
	font-size: 28px;
	font-family: Arial, sans-serif;
	color: #618ad2;
	margin-left: auto;
	margin-right: auto;
}

table {
	width: 100%;
}

input {
	width: 100%;
	text-align: right;
	font-size: 18px;
	height: auto;
}

input[type='submit'] {
	width: auto;
}

select {
	width: 100%;
	text-align-last: right;
	font-size: 18px;
	height: auto;
	border: 1px solid #ccc;
}
</style>
<script>
var weight;
var w;
var d = document;

function ScalesSocket(h, p, fm, fe) {
	let tWeight;
	let timerSocket;
	var ws;
	var startWeightTimeout = function() {
		clearTimeout(tWeight);
		tWeight = setTimeout(function() {
			fe();
		}, 5000);
	};
	this.getWeight = function() {
		ws.send('/wt');
		startWeightTimeout();
	};
	this.openSocket = function() {
		ws = new WebSocket(h, p);
		ws.onopen = this.getWeight;
		ws.onclose = function(e) {
			clearTimeout(tWeight);
			starSocketTimeout();
			fe();
		};
		ws.onerror = function(e) {
			fe();
		};
		ws.onmessage = function(e) {
			fm(JSON.parse(e.data));
		}
	};
	var starSocketTimeout = function() {
		clearTimeout(timerSocket);
		timerSocket = setTimeout(function() {
			this.prototype.openSocket();
		}, 5000);
	}
}

function go() {
	d.getElementById('wt_id').innerHTML = '---';
}

function setMax() {
	var fd = new FormData(d.getElementById('form_c_id'));
	fd.append('update', true);
	var r = new XMLHttpRequest();
	r.onreadystatechange = function() {
		if (r.readyState === 4) {
			if (r.responseText !== null) {
				if (d.getElementById("form_zero") === null) {
					d.getElementById("id_bs").value = 'ОБНОВИТЬ';
					var f = d.createElement('fieldset');
					f.id = 'form_zero';
					f.innerHTML = "<legend>Нулевой вес</legend><form action='javascript:setZero()'><p>Перед установкой убедитесь что весы не нагружены.</p><input type='submit' value='УСТАНОВИТЬ НОЛЬ'/></form><br><div id='wt_id'>---</div>";
					d.body.appendChild(f);
					setupWeight();
				}
			}
		}
	};
	r.open('POST', 'calibr.html', true);
	r.send(fd);
}

function setZero() {
	let r = new XMLHttpRequest();
	let fd = new FormData();
	fd.append('zero', true);
	fd.append('weightCal', '0');
	r.onreadystatechange = function() {
		if (r.readyState === 4 && r.status === 200) {
			if (r.responseText !== null) {
				d.getElementById('form_zero').disabled = true;
				let f = d.createElement('fieldset');
				f.id = 'form_weight';
				f.innerHTML = "<legend>Калиброваный вес</legend><form action='javascript:setWeight()'><p>Перед установкой весы нагружаются контрольным весом. Дать некоторое время для стабилизации.Значение вводится с точностью которое выбрано в пункте Точность измерения.</p><table><tr><td>ГИРЯ:</td><td><input id='gr_id' value='0' type='number' step='any' required placeholder='Калиброваная гиря'/></td></tr><tr><td>ГРУЗ:</td><td><input id='id_cal_wt' value='0' type='number' step='any' title='Введите значение веса установленого на весах' max='100000' required placeholder='Калиброваный вес'/></td></tr><tr><td>ОШИБКА:</td><td><div id='dif_gr_id'></div></td></tr><tr><td><input type='submit' value='УСТАНОВИТЬ'/></td><td><a href='javascript:calculate();'>подобрать</a></td></tr></table></form>";
				d.body.appendChild(f);
			}
		}
	};
	r.open('POST', 'calibr.html', true);
	r.send(fd);
}

function setWeight() {
	let fd = new FormData();
	let w = parseFloat(d.getElementById('id_cal_wt').value) + parseFloat(d.getElementById('gr_id').value);
	fd.append('weightCal', w.toString());
	let r = new XMLHttpRequest();
	r.onreadystatechange = function() {
		if (r.readyState === 4) {
			if (r.status === 200) {
				if (r.responseText !== null) {
					if (d.getElementById('form_seal') === null) {
						let f = d.createElement('fieldset');
						f.id = 'form_seal';
						f.innerHTML = "<legend>Пломбировка</legend><form action='javascript:setSeal()'><left><p>Сохранение процесса калибровки. Данные калибровки сохраняются в память весов.</p><input type='submit' value='ОПЛОМБИРОВАТЬ'/></left></form>";
						d.body.appendChild(f);
					}
				}
				} else if (r.status === 400) {
				alert(r.responseText);
			}
		}
	};
	r.open('POST', 'calibr.html', true);
	r.send(fd);
}

function setSeal() {
	let r = new XMLHttpRequest();
	r.onreadystatechange = function() {
		if (r.readyState === 4 && r.status === 200) {
			alert('Номер пломбы: ' + r.responseText);
			window.location.replace('/');
		}
	};
	r.open('GET', '/sl', true);
	r.send(null);
}

function GetSettings() {
	let r = new XMLHttpRequest();
	r.overrideMimeType('application/json');
	r.onreadystatechange = function() {
		if (r.readyState === 4) {
			try {
				let j = JSON.parse(r.responseText);
				for (en in j) {
					try {
						if (d.getElementById(en) !== null) d.getElementById(en).value = j[en];
						} catch (e) {}
					}
					} catch (e) {
					alert(e.toString());
				}
				d.body.style.visibility = 'visible';
			}
		};
		r.open('GET', '/cdate.json', true);
		r.send(null);
	}
	window.onload = function() {
		GetSettings();
	};

	function saveValue() {
		let fd = new FormData(d.getElementById('form_c_id'));
		fd.append('update', true);
		let r = new XMLHttpRequest();
		r.onreadystatechange = function() {
			if (r.readyState === 4 && r.status === 200) {
				if (r.responseText !== null) {
					window.open('/', '_self');
				}
			}
		};
		r.onerror = function() {
			alert('Ошибка');
		};
		r.open('POST', 'calibr.html', true);
		r.send(fd);
	};

	function getWeight() {
		w = new ScalesSocket('ws://' + d.location.host + '/ws', ['arduino'], handleWeight, function() {
			go();
			w.openSocket();
		});
		w.openSocket();
	}

	function calculate() {
		var dif = weight - parseFloat(d.getElementById('id_cal_wt').value);
		dif = parseFloat(d.getElementById('gr_id').value) / dif;
		dif = parseFloat(d.getElementById('id_cal_wt').value) * dif;
		d.getElementById('id_cal_wt').value = dif.toFixed(d.getElementById('ac_id').value);
		setWeight();
	}

	function handleWeight(dt) {
		weight = dt.w;
		d.getElementById('wt_id').innerHTML = weight;
		try {
			d.getElementById('form_seal').disabled = (dt.s === false);
			} catch (e) {}
			if (dt.s) {
				d.getElementById('wt_id').setAttribute('style', 'background: #fff;');
				} else {
				d.getElementById('wt_id').setAttribute('style', 'background: #C4C4C4;');
			}
			try {
				var dif_gr = parseFloat(d.getElementById('id_cal_wt').value) + parseFloat(d.getElementById('gr_id').value);
				dif_gr -= weight;
				d.getElementById('dif_gr_id').innerHTML = dif_gr.toFixed(d.getElementById('ac_id').value);
				} catch (e) {}
				w.getWeight();
			}

			function setupWeight() {
				getWeight();
			}
			</script>
			</head>

			<body style='visibility: hidden'><a href='/settings.html' class='btn btn--s btn--blue'>&lt;</a>&nbsp;&nbsp;<strong>Калибровка</strong>
			<hr>
			<fieldset id='form_max' style='visibility: visible'>
			<legend>Общии настройки</legend>
			<form id='form_c_id' action='javascript:setMax()'>
			<table>
			<tr>
			<td>Точность измерения</td>
			<td>
			<select id='ac_id' name='weightAccuracy' title='Выбор точности с которым будет измерения'>
			<option name='0' value='0'>0</option>
			<option name='0.0' value='1'>0.0</option>
			<option name='0.00' value='2'>0.00</option>
			<option name='0.000' value='3'>0.000</option>
			</select>
			</td>
			</tr>
			<tr>
			<td>Шаг измерения</td>
			<td>
			<select id='st_id' name='weightStep' title='Выбор шага измерения'>
			<option name='шаг 1' value='1'>1</option>
			<option name='шаг 2' value='2'>2</option>
			<option name='шаг 5' value='5'>5</option>
			<option name='шаг 10' value='10'>10</option>
			<option name='шаг 20' value='20'>20</option>
			<option name='шаг 50' value='50'>50</option>
			</select>
			</td>
			</tr>
			<tr>
			<td>Кол-во измерений</td>
			<td>
			<select id='av_id' name='weightAverage' title='Выбор количества измерений АЦП'>
			<option name='одно' value='1'>ОДИН</option>
			<option name='два' value='2'>ДВА</option>
			</select>
			</td>
			</tr>
			<tr>
			<td>Фильтр</td>
			<td>
			<select id='fl_id' name='weightFilter' title='Выбор значения фильтра. Чем меньше значение тем лутшее фильтрация но дольше измерение'>
			<option name='5%' value='5'>5 %</option>
			<option name='10%' value='10'>10 %</option>
			<option name='20%' value='20'>20 %</option>
			<option name='30%' value='30'>30 %</option>
			<option name='40%' value='40'>40 %</option>
			<option name='50%' value='50'>50 %</option>
			<option name='60%' value='60'>60 %</option>
			<option name='70%' value='70'>70 %</option>
			<option name='80%' value='80'>80 %</option>
			<option name='90%' value='90'>90 %</option>
			<option name='100%' value='100'>100 %</option>
			</select>
			</td>
			</tr>
			<tr>
			<td>НВП</td>
			<td>
			<input title='Введите значение максимального веса' type='number' min='1' max='100000' id='mw_id' name='weightMax' placeholder='Найбольший предел'>
			</td>
			</tr>
			<tr>
			<td><a href='javascript:saveValue();'>сохранить и выйти</a></td>
			<td>
			<input id='id_bs' type='submit' value='ДАЛЬШЕ >>' />
			</td>
			</tr>
			</table>
			</form>
			</fieldset>
			<fieldset>
			<details>
			<summary>Авторизация для калибровки</summary>
			<form method='POST'>
			<table>
			<tr>
			<td>ИМЯ:</td>
			<td>
			<input id='us_id' name='user' placeholder='имя админ'>
			</td>
			</tr>
			<tr>
			<td>ПАРОЛЬ:</td>
			<td>
			<input type='password' id='ps_id' name='pass' placeholder='пароль админ'>
			</td>
			</tr>
			<tr>
			<td></td>
			<td>
			<input type='submit' value='СОХРАНИТЬ' />
			</td>
			</tr>
			</table>
			</form>
			</details>
			</fieldset>
			</body>

			</html>)";

#endif







