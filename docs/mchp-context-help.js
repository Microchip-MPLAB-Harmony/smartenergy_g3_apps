( function() {  var mapping = [{"appname":"", "appid":"SE_PHY_Tester_Tool", "path":"GUID-A2A3C4CF-8C73-4F62-9809-2B710A7783A0.html"},{"appname":"", "appid":"SE_PLC_PHY_TX_Test_console", "path":"GUID-77B23EFD-7164-495F-9C75-4BE5D27443B1.html"},{"appname":"", "appid":"SE_G3_Modem", "path":"GUID-3F71506A-1159-481C-A1D4-ECC27EAB7A6B.html"},{"appname":"", "appid":"SE_MAC_RT_PLC_Go", "path":"GUID-CE3EF484-E886-4378-90B5-20C2BC7A43D1.html"},{"appname":"", "appid":"SE_PHY_PLC_Go", "path":"GUID-E87515D0-BD4F-4456-8D21-13BD460238A8.html"},{"appname":"", "appid":"SE_G3_Coordinator_UDP", "path":"GUID-BFCA859E-7CCD-47B6-9145-9FACBA1A06C3.html"},{"appname":"", "appid":"SE_G3_Coordinator_ICMP", "path":"GUID-F2A5447D-AEF1-4720-B1FA-506CC0819442.html"},{"appname":"", "appid":"SE_PHY_Sniffer_Tool", "path":"GUID-DE964FB2-4D50-4330-8A62-08DDE4711A8E.html"},{"appname":"", "appid":"SE_G3_Device_UDP", "path":"GUID-584FC808-FADD-4FBF-9DED-E84D5271738A.html"},{"appname":"", "appid":"SE_Metering_Demo_G3_Device_UDP", "path":"GUID-2EF71D08-349B-41D7-8AD9-8395EE243D44.html"}];
            var mchp = (function (mchp) {
                var mchp = mchp || {};
                var mapping = [];
        
                mchp.utils = {};
        
                mchp.utils.getQueryParam = function (name, url = window.location.href) {
                  name = name.replace(/[\[\]]/g, "\\$&");
                  var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
                    results = regex.exec(url);
                  if (!results) return null;
                  if (!results[2]) return "";
                  return decodeURIComponent(results[2].replace(/\+/g, " "));
                };

                mchp.utils.isFirefox = typeof InstallTrigger !== 'undefined';
        
                mchp.init = function (options) {
                  mchp.mapping = options.mapping || [];
                  mchp.bindEvents();
                };
        
                mchp.bindEvents = function () {
                    if (mchp.utils.isFirefox) {
                      window.onload = mchp.checkRedirect;
                    } else {
                      document.onreadystatechange = mchp.checkRedirect;
                    }
                };

                mchp.checkRedirect = function() {
                  var contextId = mchp.utils.getQueryParam("contextId") || "";
                  if (contextId && contextId != undefined) {
                    var record = mchp.mapping.find(function(x){
                      return x.appid && x.appid.toLowerCase() == contextId.toLowerCase();
                    });
                    if (record && record.path) {
                      window.stop();
                      window.location = record.path;
                    }
                  }
                };
        
                return {
                  init: mchp.init,
                };
              })();
        
              mchp.init({
                mapping: mapping
              });

        })();