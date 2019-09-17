const TempColor = '#D54242';
const HumColor = '#42D5D5';
const PressColor = '#D5D542';
const CO2Color = '#42D542';
const mainFontSize = 14;

hexMap = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'];

let ByteSizes = {
    "Dates":4,
    "Temperature":4,
    "Humidity":4,
    "AtmPressure":4,
    "CarbonDioxide":4
};
let StructSize = 0;
for(let el in ByteSizes) StructSize += ByteSizes[el];

let chart;
let Data = {
    "Dates":[],
    "Temperature":[],
    "Humidity":[],
    "AtmPressure":[],
    "CarbonDioxide":[]
};

let getCellClass = {
    "Dates":getDefaultClass,
    "Temperature":getTempCellClass,
    "Humidity":getHumCellClass,
    "AtmPressure":getDefaultClass,
    "CarbonDioxide":getCO2CellClass

};

function getData() {
    setTimeout(getData, 50000);
    let request = new XMLHttpRequest();
    request.onreadystatechange = function() {
        if (this.readyState === 4 && this.status === 200) // Запрос завершён, завершён с кодом 200 (ОК).
        {
            /*Data.Dates=[];
            Data.Temperature=[];
            Data.Humidity=[];
            Data.AtmPressure=[];
            Data.CarbonDioxide=[];//*/
            let bArray = new DataView(this.response);
            for (let i = 0; i < bArray.byteLength;)
            {
                let t = new Date(0);
                t.setSeconds(bArray.getUint32(i,true));
                Data.Dates.push(t.toGMTString().slice(5,-4));
                i+=ByteSizes.Dates;
                Data.Temperature.push(parseFloat(bArray.getFloat32(i,true).toFixed(1)));
                i+=ByteSizes.Temperature;
                Data.Humidity.push(parseFloat(bArray.getFloat32(i,true).toFixed(1)));
                i+=ByteSizes.Humidity;
                Data.AtmPressure.push(parseFloat(bArray.getFloat32(i,true).toFixed(1)));
                i+=ByteSizes.AtmPressure;
                Data.CarbonDioxide.push(bArray.getFloat32(i,true));
                i+=ByteSizes.CarbonDioxide;
            }

            // Чтобы при возникновении проблем запрос данных не останавливался
            setTimeout(UpdateChart);
            setTimeout(RebuildTable);
        }
    };
    request.open("GET", window.location.href + "getDebugData", true);
    request.responseType = "arraybuffer";
    request.send();
}

function RebuildTable() {
    let content = document.getElementById("table").innerHTML;
    let openTag = "<tbody>";
    let closeTag = "</tbody>";
    let startPos = content.indexOf(openTag) + openTag.length;
    let endPos = content.indexOf(closeTag);
    let end = content.substring(endPos,content.length);
    content = content.substring(0,startPos) + "\n";
    // insertRow вставляет элементы не в <tbody>
    for (let i = 0; i < Data.Dates.length; ++i) {
        let row = "<tr>\n";
        for (let key in Data) {
            row += "<td" + getCellClass[key](Data[key][i]) + ">";
            row += (Data[key][i]);
            row += "</td>";
        }
        row += "</tr>\n";
        end = row+end;
    }
    content += end;
    document.getElementById("table").innerHTML = content;
}

function getTempCellClass(val) {
    if (val < 0) return " class=\"bg-primary\"";
    if (val < 15) return " class=\"bg-info\"";
    if (val > 25) return " class=\"bg-warning\"";
    if (val > 35) return " class=\"bg-danger\"";
    return ""
}

function getHumCellClass(val) {
    if (val < 30) return " class=\"bg-danger\"";
    if (val < 40) return " class=\"bg-warning\"";
    if (val > 70) return " class=\"bg-danger\"";
    if (val > 60) return " class=\"bg-warning\"";
    return ""
}

function getDefaultClass(val) {
    return ""
}

function getCO2CellClass(val) {
    if (val > 1400) return " class=\"bg-danger\"";
    if (val > 1000) return " class=\"bg-warning\"";
    if (val > 800) return " class=\"bg-success\"";
    return ""
}

/**
 * @return {string}
 */
function ToHex(bArray){
    let hex = '';
    for (let i = 0; i < bArray.length; ++i) {
        hex += hexMap[(bArray[i] & 0xF0) >> 4];
        hex += hexMap[bArray[i] & 0x0F];
    }
    return hex;
}

function DrawChart(){
    chart = new Chart(document.getElementById('chart'), {
        type: 'line',
        data: {
            labels: Data.Dates,
            datasets: [{
                label: 'Температура',
                yAxisID: 'T',
                pointBackgroundColor: TempColor,
                borderColor: TempColor,
                data: Data.Temperature,
                fill: false
            }, {
                label: 'Относительная влажность',
                yAxisID: 'H',
                pointBackgroundColor: HumColor,
                borderColor: HumColor,
                data: Data.Humidity,
                fill: false
            }, {
                label: 'Атмосферное давление',
                yAxisID: 'P',
                pointBackgroundColor: PressColor,
                borderColor: PressColor,
                data: Data.AtmPressure,
                fill: false
            }, {
                label: 'Содержание CO2',
                yAxisID: 'D',
                pointBackgroundColor: CO2Color,
                borderColor: CO2Color,
                data: Data.CarbonDioxide,
                fill: false
            }]
        },
        options: {
            responsive: true,
            responsiveAnimationDuration: 0,
            maintainAspectRatio: true,
            aspectRatio: 2,
            legend: {
                position: 'top',
                labels: {
                    fontColor: 'white',
                    fontSize: mainFontSize
                }
            },
            title: {
                display: true,
                text: 'Метеоданные',
                fontColor: 'white',
                fontSize: mainFontSize * 1.5
            },
            scales: {
                xAxes: [{
                    ticks: {
                        fontColor: 'white',
                        fontSize: mainFontSize
                    },
                    pointLabels: {
                        fontColor: 'white',
                        fontSize: mainFontSize
                    },
                    gridLines: {
                        color: 'rgba(255, 255, 255, 0.2)'
                    }
                }],
                yAxes: [{
                    id: 'T',
                    type: 'linear',
                    position: 'left',
                    scaleLabel: {
                        display: true,
                        labelString: 'Температура, °C',
                        fontColor: TempColor,
                        fontSize: mainFontSize
                    },
                    ticks: {
                        fontColor: TempColor,
                        fontSize: mainFontSize
                    },
                    pointLabels: {
                        fontColor: TempColor,
                        fontSize: mainFontSize
                    },
                    gridLines: {
                        color: 'rgba(255, 255, 255, 0.2)'
                    }
                }, {
                    id: 'H',
                    type: 'linear',
                    position: 'left',
                    scaleLabel: {
                        display: true,
                        labelString: 'Влажность, %',
                        fontColor: HumColor,
                        fontSize: mainFontSize
                    },
                    ticks: {
                        fontColor: HumColor,
                        fontSize: mainFontSize
                    },
                    pointLabels: {
                        fontColor: HumColor,
                        fontSize: mainFontSize
                    },
                    gridLines: {
                        color: 'rgba(255, 255, 255, 0.2)'
                    }
                }, {
                    id: 'P',
                    type: 'linear',
                    position: 'right',
                    scaleLabel: {
                        display: true,
                        labelString: 'АД, mm Hg',
                        fontColor: PressColor,
                        fontSize: mainFontSize
                    },
                    ticks: {
                        fontColor: PressColor,
                        fontSize: mainFontSize
                    },
                    pointLabels: {
                        fontColor: PressColor,
                        fontSize: mainFontSize
                    },
                    gridLines: {
                        color: 'rgba(255, 255, 255, 0.2)'
                    }
                }, {
                    id: 'D',
                    type: 'linear',
                    position: 'right',
                    scaleLabel: {
                        display: true,
                        labelString: 'CO2, ppm',
                        fontColor: CO2Color,
                        fontSize: mainFontSize
                    },
                    ticks: {
                        fontColor: CO2Color,
                        fontSize: mainFontSize
                    },
                    pointLabels: {
                        fontColor: CO2Color,
                        fontSize: mainFontSize
                    },
                    gridLines: {
                        color: 'rgba(255, 255, 255, 0.2)'
                    }
                }]
            },
            pan: {
                enabled: true,
                mode: "xy"
                // Function called while the user is panning
                // onPan: function({chart}) { console.log(`I'm panning!!!`); },
                // Function called once panning is completed
                // onPanComplete: function({chart}) { console.log(`I was panned!!!`); }
            },
            zoom: {
                enabled: true,
                drag: false,
                mode: "xy"
                // Function called while the user is zooming
                // onZoom: function({chart}) { console.log(`I'm zooming!!!`); },
                // Function called once zooming is completed
                // onZoomComplete: function({chart}) { console.log(`I was zoomed!!!`); }
            },
            annotation:{
                annotations: [{
                    type: 'line',
                    mode: 'vertical',
                    scaleID: 'x-axis',
                    //borderColor: '#b6fcd5',
                    borderWidth: 2
                }]
            },
            tooltips: {
                mode: 'x',
                intersect: false
            }
        }
    });
}

function UpdateChart(){
    chart.data.labels = Data.Dates;
    chart.data.datasets[0].data = Data.Temperature;
    chart.data.datasets[1].data = Data.Humidity;
    chart.data.datasets[2].data = Data.AtmPressure;
    chart.data.datasets[3].data = Data.CarbonDioxide;
    chart.options.scales.yAxes[0].ticks.min = Math.round(Math.min.apply(null, Data.Temperature)*0.9);
    chart.options.scales.yAxes[0].ticks.max = Math.round(Math.max.apply(null, Data.Temperature)*1.1);
    chart.options.scales.yAxes[1].ticks.min = Math.round(Math.min.apply(null, Data.Humidity)*0.9);
    chart.options.scales.yAxes[1].ticks.max = Math.round(Math.max.apply(null, Data.Humidity)*1.1);
    chart.options.scales.yAxes[2].ticks.min = Math.round(Math.min.apply(null, Data.AtmPressure)*0.9);
    chart.options.scales.yAxes[2].ticks.max = Math.round(Math.max.apply(null, Data.AtmPressure)*1.1);
    chart.options.scales.yAxes[3].ticks.min = Math.round(Math.min.apply(null, Data.CarbonDioxide)*0.9);
    chart.options.scales.yAxes[3].ticks.max = Math.round(Math.max.apply(null, Data.CarbonDioxide)*1.1);
    chart.update();
}

function ResetZoom() {
    chart.resetZoom();
}

document.addEventListener('DOMContentLoaded', function(){
    DrawChart();
    getData();
});