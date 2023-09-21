#ifndef HTML_CONTENT_H
#define HTML_CONTENT_H
#include "config.h"

extern String output2State;

String controlHtml()
{
  String html = R"(
  <!DOCTYPE html>
  <html>
  <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link rel="icon" href="data:,">
      <style>
          html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            padding: 10vh 0;
            text-align: center;
            background-color: #3a3a3a;
            color: white;
          }

          a {
            color: grey;
          }

          .button {
            background-color: #4CAF50;
            border: none;
            border-radius: 10px;
            color: white;
            padding: 30px 30px;
            text-decoration: none;
            font-size: 30px;
            margin: 2px;
            cursor: pointer;
          }

          .button2 {
            background-color: #555555;
          }

          p {
            font-size: 14px;
          }

          .container-item {
            display: flex;
            flex-direction: row;
            justify-content: center;
            align-items: center;
            gap: 20px;
          }
          .state-pin{
            font-size: 18px;
          }
          .state-pin span {
            font-size: 30px;
          }
      </style>
  </head>
  <body>
    <h1>Control Board</h1>
    <div class="container-item">)";

  if (output2State == "off")
  {
    html += R"(
      <p class="state-pin">GPIO 2 <br> <span>OFF</span></p>
      <p><a href="/pins-2/on"><button class="button">ON</button></a></p>
    )";
  }
  else
  {
    html += R"(
      <p class="state-pin">GPIO 2 <br> <span>ON</span></p>
      <p><a href="/pins-2/off"><button class="button button2">OFF</button></a></p>
    )";
  }

  html += R"(
    </div>
    <div>
      <p>Enter 5 digits. Integer (1 >= number <= 9999), 1 digit (dot location).</p>
      <form method='POST' action='#' id='number_form' class='container-item'>
        <input type='text' name='number' class='button button2' id='inputNumb' placeholder="xxx.x" required
          pattern="^\d{1,4}(\.\d{1,3})?$">
        <input type='submit' value='Apply' class='button'>
      </form>
    </div>
    <p><a href="/">Go home...</a></p>
    <script>
      document.querySelector("#inputNumb").addEventListener("input", function () {
        document.querySelector("#inputNumb").addEventListener("input", function () {
          let cleanedValue = this.value.replace(/[^\d.]/g, "");
          if (cleanedValue.includes('.')) cleanedValue = cleanedValue.substring(0, 5);
          else cleanedValue = cleanedValue.substring(0, 4);

          if (cleanedValue.includes('.') && !cleanedValue.startsWith('.')) {
            this.value = cleanedValue;
            if (cleanedValue.length == 5 && cleanedValue.endsWith('.')) this.value = cleanedValue.replace('.', '');

            if (cleanedValue.indexOf('.', cleanedValue.indexOf('.') + 1) !== -1) {
              this.value = cleanedValue.slice(0, cleanedValue.indexOf('.') + 1) + cleanedValue.slice(cleanedValue.indexOf('.') + 1).replace('.', '');
            }
          } else this.value = cleanedValue.replace('.', '');
        });
      });
      document.querySelector('#number_form').addEventListener('submit', (e) => {
        e.preventDefault();
        let number = e.target[0].value.replace('.', '');
        let positionPoint = e.target[0].value.includes('.') ? (number.length - e.target[0].value.lastIndexOf('.')) : 0;
        window.location.assign(`/number-UPD?number=${number}&point=${positionPoint}`);
        e.target[0].value = '';
      })
    </script>
    </body>
    </html>
  )";

  return html.c_str();
}

String updateOTAHTML()
{
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {
      font-family: Helvetica;
      display: inline-block;
      margin: 0px auto;
      padding: 10vh 0;
      text-align: center;
      background-color: #3a3a3a;
      color: white;
    }

    body {
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
    }

    a {
      color: grey;
    }
    
    .button {
      background-color: #4CAF50;
      border: none;
      border-radius: 10px;
      color: white;
      padding: 40px 40px;
      text-decoration: none;
      font-size: 30px;
      margin: 20px;
      cursor: pointer;
    }
    
    .input {
      border-radius: 10px;
      padding: 40px 40px;
      text-decoration: none;
      font-size: 30px;
      cursor: pointer;
    }

    .input::-webkit-file-upload-button {
      background-color: #555555;
      border: none;
      border-radius: 10px;
      color: white;
      padding: 20px 20px;
      text-decoration: none;
      font-size: 30px;
      cursor: pointer;
    }

    .load-container {
      position: relative;
      width: 50vw;
      height: 5vh;
      background-color: #555555;
      border: none;
      border-radius: 10px;
      color: white;
      padding: 10px 10px;
      text-decoration: none;
      font-size: 30px;
      margin: 20px;
      overflow: hidden;
    }

    #prg {
      position: relative;
      width: 100%;
      height: 100%;
      line-height: 130%;
      z-index: 10;
    }

    .load {
      position: absolute;
      top: 0;
      left: 0;
      width: 0%;
      height: 100%;
      background-color: #4CAF50;
      z-index: 8;
    }

  </style>
  <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
</head>
<body>
  <form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
    <input type='file' name='update' class='input'> 
    <input type='submit' value='Update' class='button'> 
  </form>
  <div class="load-container button">
    <div id='prg'>0%</div>
    <div class="load"></div>
  </div>
  <p><a href="/">Go home...</a></p>
  <script>

  $('form').submit(function (e) {
    e.preventDefault();
    var form = $('#upload_form')[0];
    var data = new FormData(form);
    $.ajax({
      url: '/update',
      type: 'POST',
      data: data,
      contentType: false,
      processData: false,
      xhr: function () {
        var xhr = new window.XMLHttpRequest();
        xhr.upload.addEventListener('progress', function (evt) {
          if (evt.lengthComputable) {
            var per = evt.loaded / evt.total;
            $('#prg').html(Math.round(per * 100) + '%');
            $('.load').css('width', Math.round(per * 100) + '%');
          }
        }, false);
        return xhr;
      },
      success: function (d, s) {
        console.log('success!')
      },
      error: function (a, b, c) {
      }
    });
  });

  </script>
</body>
</html>
)";
  return html.c_str();
}

String loginHTML()
{
  String html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {
      font-family: Helvetica;
      display: inline-block;
      margin: 0px auto;
      padding: 10vh 0;
      text-align: center;
      background-color: #3a3a3a;
      color: white;
    }

    a {
      color: grey;
    }

    .button {
      background-color: #4CAF50;
      border: none;
      border-radius: 10px;
      color: white;
      padding: 40px 40px;
      text-decoration: none;
      font-size: 30px;
      margin: 20px;
      cursor: pointer;
    }
  </style>
</head>
<body>
  <h1>Control Pins</h1>
  <p>Select the function:</p>
  <div>
    <a href='/update-OTA'><button class='button'>Upload Sketch</button></a>
    <a href='/control-pins'><button class='button'>Control Pins</button></a>
  </div>
</html>
)";
  return html.c_str();
}

#endif