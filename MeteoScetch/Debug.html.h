#pragma once

const char* DebugPage PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
  <meta charset="utf-8">
  <title>Метеостанция</title>
</head>

<body>
  <p id="Mem">Mem usage: </p>
  <table id="table" border="1">
  <thead>
    <tr>
      <th>Дата и время</th>
      <th>Температура, °C</th>
      <th>Относительная влажность, %</th>
      <th>Атмосферное давление, mm Hg</th>
      <th>Содержание CO2, ppm</th>
    </tr>
  </thead>
  <tbody>
  </tbody>
  </table>
  <script>
	let ByteSizes = {
		"Dates":4,
		"Temperature":4,
		"Humidity":4,
		"AtmPressure":4,
		"CarbonDioxide":4
	};

    function getData() {
		setTimeout(getData, 1000);
      var request = new XMLHttpRequest();
      request.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) // Запрос завершён, завершён с кодом 200 (ОК).
        {
			let content = document.getElementById("table").innerHTML;
			let openTag = "<tbody>";
			let closeTag = "</tbody>";
			let startPos = content.indexOf(openTag) + openTag.length;
			let endPos = content.indexOf(closeTag);
			let end = content.substring(endPos,content.length);
			content = content.substring(0,startPos) + "\n";

            let bArray = new DataView(this.response);
            for (let i = 0; i < bArray.byteLength;)
            {
				let row = "<tr>\n";

                let t = new Date(0);
                t.setSeconds(bArray.getUint32(i,true));
				row += "<td>";
				row += (t.toGMTString().slice(5,-4));
				row += "</td>";
                i+=ByteSizes.Dates;

				row += "<td>";
				row += (bArray.getFloat32(i,true));
				row += "</td>";
                i+=ByteSizes.Temperature;

				row += "<td>";
				row += (bArray.getFloat32(i,true));
				row += "</td>";
                i+=ByteSizes.Humidity;

				row += "<td>";
				row += (bArray.getFloat32(i,true));
				row += "</td>";
                i+=ByteSizes.AtmPressure;

				row += "<td>";
				row += (bArray.getFloat32(i,true));
				row += "</td>";
                i+=ByteSizes.CarbonDioxide;

				row += "</tr>\n";
				end = row+end;
            }
			
			content += end;
			document.getElementById("table").innerHTML = content;
        }
      };
      request.open("GET", "getDebugData", true);
      request.responseType = "arraybuffer";
      request.send();
    }

    hexMap = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'];

    function ToHex(bArray) {
      var hex = '';
      for (i = 0; i < bArray.length; ++i) {
        hex += hexMap[(bArray[i] & 0xF0) >> 4];
        hex += hexMap[bArray[i] & 0x0F];
      }
      return hex;
    }

document.addEventListener('DOMContentLoaded', function(){
    getData();
});
  </script>
</body>

</html>
)=====";