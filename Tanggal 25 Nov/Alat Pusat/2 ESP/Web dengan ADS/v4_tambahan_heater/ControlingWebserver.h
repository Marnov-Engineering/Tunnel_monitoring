const char controling_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
  <meta charset="UTF-8">
  <title>Monitoring Tunnel</title>
  <script src="/highcharts.js"></script>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      background: linear-gradient(to right, #f0f4f8, #d9e2ec);
      margin: 0;
      padding: 0px;
      color: #333;
    }

    header {
      background-color: #2f80ed;
      color: white;
      padding: 30px 0;
      text-align: center;
      box-shadow: 0 2px 6px rgba(0,0,0,0.1);
    }

    h2 { margin: 0; font-size: 24px; }
    main { max-width: 900px; margin: auto; }

    .chart-container {
      margin-top: 30px;
      margin-bottom: 30px;
    }

    .chart-box {
      width: 100%;
      height: 400px;
      background-color: white;
      border-radius: 8px;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
      padding: 10px;
    }

    footer {
      text-align: center;
      padding: 15px;
      color: #777;
      font-size: 14px;
    }

    form, .control-section {
      margin: 20px 0;
      background: #fff;
      padding: 15px;
      border-radius: 8px;
      box-shadow: 0 1px 5px rgba(0,0,0,0.1);
      justify-content: center;
      align-items: center;
    }

    input, button, select {
      padding: 8px;
      margin: 5px;
      font-size: 14px;
    }

    hr { margin-top: 20px; margin-bottom: 20px; }
  </style>
</head>
<body>
  <header>
    <h2>Monitoring Tunnel</h2>
  </header>

  <main>
    <div class="chart-container"><div id="chart1" class="chart-box"></div></div>
    <div class="chart-container"><div id="chart2" class="chart-box"></div></div>

    <!-- Form Kontrol dan Kalibrasi -->
    <div style="text-align: center; max-width: 500px; margin: 0 auto;">
      <div class="control-section">
        <h3>ESP32 Web Server Controlling</h3>

        <p><b>Set Tunnel Temperature (PID) :</b></p>
        <form action="/get">
          <input type="number" name="In_Suhu_Tunnel" placeholder="Input Suhu Tunnel">
          <input type="submit" value="Submit">
        </form>
        <p>Temperature = <span id="Suhu_Tunnel">0</span> °C</p>
        <!-- <p>Kecepatan = <span id="Kecepatan">0</span> %</p> -->
      </div>

      <div class="control-section">
        <p><b>Fan Safety:</b></p>
        <form action="/get">
          <label for="In_Heat_st">Fan :</label>
          <select name="In_Heat_st" id="In_Heat_st">
            <option value="1">ON</option>
            <option value="0">OFF</option>
          </select>
          <input type="submit" value="Submit">
        </form>
        <p>Fan = <span id="Heat_st">-</span></p>
      </div>

    <div class="control-section">
      <p><b>Fan Power:</b></p>
      <form action="/get">
        <label for="In_Fan_Power">Fan :</label>
        <select name="In_Fan_Power" id="In_Fan_Power">
          <option value="1">Max</option>
          <option value="0">OFF</option>
        </select>
        <input type="submit" value="Submit">
      </form>
      <p>Fan = <span id="Fan_Power">-</span></p>
    </div>

    <div class="control-section">
      
      <p><b>Set Heater Power:</b></p>
      <form action="/get">
        <input type="number" name="In_Power_Tunnel" placeholder="Input Power Heater">
        <input type="submit" value="Submit">
      </form>
      <p>Power = <span id="Power_Tunnel">0</span> %</p>
      <!-- <p>Kecepatan = <span id="Kecepatan">0</span> %</p> -->
    </div>

      <!-- <div class="control-section">
        <p><b>Heater:</b></p>
        <form action="/get">
          <label for="In_Heat_st">Heater :</label>
          <select name="In_Heat_st" id="In_Heat_st">
            <option value="1">ON</option>
            <option value="0">OFF</option>
          </select>
          <input type="submit" value="Submit">
        </form>
        <p>Heater = <span id="Heat_st">-</span></p>
      </div> -->

      <div class="control-section">

        <p><b>BME280 Callibration:</b></p>
        <p>Temperature = <span id="DindSuhu1">0</span>  °C</p>

        <p>Input Kalibrasi Sensor BME280 Dinding 1 (y = mx + b)</p>
        <form action="/get">
          <input type="number" name="In_CalM_DinD1" step="0.00001" placeholder="Input M">
          <input type="submit" value="Submit">
        </form>
        <form action="/get">
          <input type="number" name="In_CalB_DinD1" step="0.00001" placeholder="Input B">
          <input type="submit" value="Submit">
        </form>
        <p>Nilai M = <span id="CalM_DinD1">1.00000</span>, Nilai B = <span id="CalB_DinD1">0.00000</span></p>

        <hr>
        <p>Temperature = <span id="DindSuhu2">0</span>  °C</p>
        <p>Input Kalibrasi Sensor BME280 Dinding 2 (y = mx + b)</p>
        <form action="/get">
          <input type="number" name="In_CalM_DinD2" step="0.00001" placeholder="Input M">
          <input type="submit" value="Submit">
        </form>
        <form action="/get">
          <input type="number" name="In_CalB_DinD2" step="0.00001" placeholder="Input B">
          <input type="submit" value="Submit">
        </form>
        <p>Nilai M = <span id="CalM_DinD2">1.00000</span>, Nilai B = <span id="CalB_DinD2">0.00000</span></p>

        <hr>
        <p>Temperature = <span id="DindSuhu3">0</span>  °C</p>
        <p>Input Kalibrasi Sensor BME280 Dinding 3 (y = mx + b)</p>
        <form action="/get">
          <input type="number" name="In_CalM_DinD3" step="0.00001" placeholder="Input M">
          <input type="submit" value="Submit">
        </form>
        <form action="/get">
          <input type="number" name="In_CalB_DinD3" step="0.00001" placeholder="Input B">
          <input type="submit" value="Submit">
        </form>
        <p>Nilai M = <span id="CalM_DinD3">1.00000</span>, Nilai B = <span id="CalB_DinD3">0.00000</span></p>

      </div>
      
      <div class="control-section">
        <p><b>Menu Monitoring</b></p>
        <button onclick="window.location.href='/'">Ke Halaman Monitoring</button>
      </div>
  
  </main>

  <footer>
    &copy; 2025 | Sistem Monitoring Tunnel ESP32
  </footer>

  <!-- JavaScript -->
  <script>
    Highcharts.setOptions({ time: { useUTC: false } });

    const chart2 = Highcharts.chart('chart2', {
      chart: { type: 'spline' },
      title: { text: 'Grafik Suhu Dinding (BME280)' },
      xAxis: { type: 'datetime', title: { text: 'Waktu' } },
      yAxis: { title: { text: 'Tekanan' } },
      tooltip: { xDateFormat: '%H:%M:%S', shared: true },
      series: [
        { name: 'Suhu Dinding 1', data: [] },
        { name: 'Suhu Dinding 2', data: [] },
        { name: 'Suhu Dinding 3', data: [] }
      ]
    });

    const chart1 = Highcharts.chart('chart1', {
      chart: { type: 'spline' },
      title: { text: 'Grafik Suhu Dalam Pipa (BME280)' },
      xAxis: { type: 'datetime', title: { text: 'Waktu' } },
      yAxis: { title: { text: 'Suhu' } },
      tooltip: { xDateFormat: '%H:%M:%S', shared: true },
      series: [
        { name: 'Suhu Dalam Pipa', data: [] }
      ]
    });

    async function fetchData() {
      try {
        const res = await fetch('/dataDin');
        const data = await res.json();
        const x = Date.now();
        const y = data.temperature;

        // chart2.series[0].addPoint([x, data.Dsuhu1], true, chart2.series[0].data.length > 30);
        // chart2.series[1].addPoint([x, data.Dsuhu2], true, chart2.series[1].data.length > 30);
        // chart2.series[2].addPoint([x, data.Dsuhu3], true, chart2.series[2].data.length > 30);

        chart2.series[0].addPoint([x, data.Dsuhu1]);
        chart2.series[1].addPoint([x, data.Dsuhu2]);
        chart2.series[2].addPoint([x, data.Dsuhu3]);


        chart1.series[0].addPoint([x, data.BME280]);

        // Jika data.suhu adalah array

      } catch (error) {
        console.error('Gagal mengambil data:', error);
      }
    }

    setInterval(fetchData, 2000);

    // Ambil data settingan realtime
    setInterval(function () {
      const xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
          const data = this.responseText.split(',');
          
          document.getElementById("Suhu_Tunnel").innerHTML = data[0];

          document.getElementById("Power_Tunnel").innerHTML = data[1];

          document.getElementById("Heat_st").innerHTML = (data[2] === "1") ? "ON" : "OFF";

          document.getElementById("Fan_Power").innerHTML = (data[3] === "1") ? "MAX" : "OFF";

          document.getElementById("CalM_DinD1").innerHTML = parseFloat(data[4]).toFixed(5);
          document.getElementById("CalB_DinD1").innerHTML = parseFloat(data[5]).toFixed(5);

          document.getElementById("CalM_DinD2").innerHTML = parseFloat(data[6]).toFixed(5);
          document.getElementById("CalB_DinD2").innerHTML = parseFloat(data[7]).toFixed(5);

          document.getElementById("CalM_DinD3").innerHTML = parseFloat(data[8]).toFixed(5);
          document.getElementById("CalB_DinD3").innerHTML = parseFloat(data[9]).toFixed(5);

          document.getElementById("DindSuhu1").innerHTML = parseFloat(data[10]).toFixed(2);
          document.getElementById("DindSuhu2").innerHTML = parseFloat(data[11]).toFixed(2);
          document.getElementById("DindSuhu3").innerHTML = parseFloat(data[12]).toFixed(2);
          
        }
      };
      xhttp.open("GET", "/dataSetHeat", true);
      xhttp.send();
    }, 1000);G

    function updateServer() {
      const xhttp = new XMLHttpRequest();
      xhttp.open("GET", "/updateServer", true);
      xhttp.send();
    }

  </script>
</body>
</html>


)rawliteral";