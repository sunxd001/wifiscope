
void rootPage() //root page: http://192.168.4.1/
{
  if(Server.arg("s") != NULL)
  {
    Server.send_P(200, "image/jpeg", (const char *)ucBuffer, 1000);

    uiBuffer = 0;
  }
  else
  {
    PGM_P content = PSTR("\
<html>\r\n\
<head>\r\n\
<title>WiFi Oscilliscope</title>\r\n\
<style type='text/css'>\r\n\
body{background:#00E0F0;color:#777}\r\n\
.container{max-width:95%;width:100%;margin:0 auto}\r\n\
.content{margin:10px}#C{width:100%}\r\n\
.inner{background:#F9F9F9;padding:30px 25px;margin:70px 0;box-shadow:0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0,0,0,0.24)}\r\n\
input{width:100%;margin:5px 0 10px;padding:10px}\r\n\
</style>\r\n\
</head>\r\n\
<body>\r\n\
  <div class='container'>\r\n\
    <div class='inner'>\r\n\
    <div style='text-align:center'> \r\n\
      Wifi Oscilliscope\r\n\
    </div>\r\n\
      <div class='content'> \r\n\
      <canvas id='C' width='1000' height='256' style='border:1px solid #d3d3d3;'> Your browser does not support the HTML5 canvas tag. </canvas>\r\n\
    </div>\r\n\
      <div id='num'> \r\n\
      <input type='button' id='btn' onclick='btn();' value='10KSPS(100mS)'>\r\n\
    </div>\r\n\
  </div>\r\n\
  </div> \r\n\
  <script>\r\n\
    var mode=0;\r\n\
  var c=document.getElementById('C');\r\n\
  var b=document.getElementById('btn');\r\n\
  var ctx=c.getContext('2d');\r\n\
  var olddata;\r\n\
  var intval=setInterval(loadFrame,1000);\r\n\
  function btn(){\r\n\
      if(mode==0){\r\n\
      b.value='0.5KSPS(2S)';\r\n\
      mode=1;\r\n\
    } \r\n\
    else {\r\n\
      b.value='10KSPS(100mS)';\r\n\
      mode=0;\r\n\
    }\r\n\
  } \r\n\
  function loadFrame(){\r\n\
      clearInterval(intval);\r\n\
    if(window.XMLHttpRequest) \r\n\
      xhr=new XMLHttpRequest();\r\n\
    else \r\n\
      xhr=new ActiveXObject('Microsoft.XMLHTTP');\r\n\
      xhr.open('GET','?s=1',true);\r\n\
      xhr.responseType='arraybuffer';\r\n\
      xhr.setRequestHeader('Connection','keep-alive');\r\n\
      xhr.onreadystatechange=function() {\r\n\
        if(xhr.readyState==4 && xhr.status==200) {\r\n\
        var adc=new Uint8Array(xhr.response);\r\n\
        var j=adc.length;\r\n\
        if(j==1000) {\r\n\
          ctx.clearRect(0,0,1000,256);\r\n\
        if(mode==0||mode==1) {\r\n\
          for(i=0;i<999;i++) {\r\n\
            ctx.beginPath();\r\n\
          ctx.moveTo(i,(256-adc[i]));\r\n\
          ctx.lineTo(i+1,(256-adc[i+1]));\r\n\
          ctx.stroke();\r\n\
          } \r\n\
          if(mode==1) {\r\n\
            mode=2;\r\n\
          olddata=adc;\r\n\
          }\r\n\
        } else {\r\n\
          for(i=100;i<1000;i++) \r\n\
            olddata[i-100]=olddata[i];\r\n\
          var l=0;\r\n\
          for(i=900;i<1000;i++) {\r\n\
            var k,m=0;\r\n\
          for(k=0;k<10;k++) {\r\n\
            m+=adc[l];\r\n\
            l++;\r\n\
          } \r\n\
          m/=10;\r\n\
          olddata[i]=255-m;\r\n\
          } \r\n\
          for(i=0;i<999;i++) {\r\n\
            ctx.beginPath();\r\n\
          ctx.moveTo(i,olddata[i]);\r\n\
          ctx.lineTo(i+1,olddata[i+1]);\r\n\
          ctx.stroke();\r\n\
          }//for i\r\n\
         }//else mode=0 || mode=1\r\n\
       }//if j==1000 \r\n\
       intval=setInterval(loadFrame,50);\r\n\
      }//if status==200\r\n\
      };//onreadystatchange\r\n\
      xhr.send();\r\n\
    }</script> \r\n\
  </body>\r\n\
</html>\r\n\
    ");
    Server.send(200, "text/html", content);
  }
}
