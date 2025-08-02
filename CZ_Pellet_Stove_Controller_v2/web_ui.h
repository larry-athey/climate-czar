//------------------------------------------------------------------------------------------------
// CZ Pellet Stove Controller v2 | (CopyLeft) 2022-Present | Larry Athey (https://panhandleponics.com)
//
// Inline functions used for modular unit organization
//------------------------------------------------------------------------------------------------
inline String AjaxRefreshJS(String AjaxID,String Query,String RefreshMS) { // Refreshes data in a card on a random timed basis
  String Content = "";
  Content += "\n<script type=\"text/javascript\">\n";
  Content += "  jQuery(document).ready(function() {\n";
  Content += "    RandomDelay = " + RefreshMS + " + Math.floor(Math.random() * 1000) + 1;\n";
  Content += "    function refresh() {\n";
  Content += "      jQuery('#" + AjaxID + "').load('./" + Query + "');\n";
  Content += "    }\n";
  Content += "    setInterval(function() {\n";
  Content += "      refresh()\n";
  Content += "    },RandomDelay);\n";
  Content += "  });\n";
  Content += "</script>\n";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String CreateLink(String LinkText,String FormTitle,String FormID) { // Creates a link for editing a setting
  return "<a href=\"javascript:void(0);\" onClick=\"LoadForm('" + FormTitle + "','" + FormID + "')\"><span class=\"text-info\">" + LinkText + "</span></a>";
}
//------------------------------------------------------------------------------------------------
inline String CreateModal() { // Bootstrap modal used as a popover container for forms
  String Content = "";
  Content += "<div class=\"modal fade\" id=\"dynamicModal\" tabindex=\"-1\" aria-labelledby=\"dynamicModalLabel\" aria-hidden=\"true\">";
  Content +=   "<div class=\"modal-dialog\">";
  Content +=     "<div class=\"modal-content\">";
  Content +=       "<div class=\"modal-header\">";
  Content +=         "<h5 class=\"modal-title\" id=\"dynamicModalLabel\">Form Title</h5>";
  Content +=         "<button type=\"button\" class=\"btn-close\" data-bs-dismiss=\"modal\" aria-label=\"Close\"></button>";
  Content +=       "</div>";
  Content +=       "<div id=\"modalContent\" class=\"modal-body\">";
  Content +=         "<div id=\"form-content\">Loading...</div>";
  Content +=       "</div>";
  Content +=       "<div class=\"modal-footer\" style=\"vertical-align: bottom;\">";
  Content +=         "<button type=\"button\" class=\"btn btn-sm btn-warning fw-bolder\" id=\"submit_button\"> Save </button>";
  Content +=       "</div>";
  Content +=     "</div>";
  Content +=   "</div>";
  Content += "</div>\n\n";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String DrawCard(String Body,String AjaxID,String Query,bool DoAjax) { // Plots a Bootstrap card
  String Content = "";
  if (DoAjax) Content += AjaxRefreshJS(AjaxID,Query,"4000") + "\n";
  Content += "<div class=\"card\" style=\"width: 98%; margin-top: 0.5em; margin-bottom: 0.5em; margin-left: 0.5em; margin-right: 0.5em;\">\n";
  Content +=   "<div class=\"card-body\">\n";
  Content +=     "<div id=\"" + AjaxID + "\">\n";
  Content +=      Body + "\n";
  Content +=     "</div>\n";
  Content +=   "</div>\n";
  Content += "</div>\n";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String getForm(byte WhichOne) { // Dynamically creates the form for the specified setting
  String Content = "";
  String Label,Name,Value,Step,Min,Max;

  if (WhichOne == 0) {
    Label = "Select Mode";
    Name = "temp-mode";
  } else if (WhichOne == 1) {
    Label = "Use internal thermostat?";
    Name = "thermostat";
  } else if (WhichOne == 2) {
    Name = "therm-temp";
    if (TemperatureMode == 0) {
      Label = "32 to 100";
      Step = ".1"; Min = "32"; Max = "100"; Value = String(targetTempF,1);
    } else {
      Label = "0 to 38";
      Step = ".1"; Min = "0"; Max = "38"; Value = String(targetTempC,1);
    }
  } else if (WhichOne == 3) {
    Name = "min-temp";
    if (TemperatureMode == 0) {
      Label = "90 to 120";
      Step = ".1"; Min = "90"; Max = "120"; Value = String(minTempF,1);
    } else {
      Label = "32 to 49";
      Step = ".1"; Min = "32"; Max = "49"; Value = String(minTempC,1);
    }
  } else if (WhichOne == 4) {
    Name = "max-temp";
    if (TemperatureMode == 0) {
      Label = "120 to 500";
      Step = ".1"; Min = "120"; Max = "500"; Value = String(maxTempF,1);
    } else {
      Label = "49 to 260";
      Step = ".1"; Min = "49"; Max = "260"; Value = String(maxTempC,1);
    }
  } else if (WhichOne == 5) {
    Name = "startup-timer";
    Label = "300 to 3000";
    Step = "1"; Min = "300"; Max = "3000"; Value = String(StartupTimer);
  } else if (WhichOne == 6) {
    Name = "feed-low";
    Label = "1 to 10";
    Step = ".1"; Min = "1"; Max = "10"; Value = String(feedRateLow,1);
  } else if (WhichOne == 7) {
    Name = "feed-high";
    Label = "1 to 10";
    Step = ".1"; Min = "1"; Max = "10"; Value = String(feedRateHigh,1);
  }

  Content += "<form id=\"modalForm\" onsubmit=\"return false;\">";
  Content += "<label for=\"" + Name + "\" class=\"form-label\">" + Label + "</label>";
  if (WhichOne == 0) {
    String S0,S1;
    if (TemperatureMode == 0) {
      S0 = "selected";
      S1 = "";
    } else {
      S0 = "";
      S1 = "selected";
    }
    Content += "<select style=\"width: 100%;\" size=\"1\" class=\"form-control form-select fw-bolder\" id=\"temp-mode\" name=\"temp-mode\">";
    Content += "<option " + S0 + " value=\"f\">Fahrenheit</option>";
    Content += "<option " + S1 + " value=\"c\">Celcius</option>";
    Content += "</select>";
  } else if (WhichOne == 1) {
    String S0,S1;
    if (UseThermostat) {
      S0 = "";
      S1 = "selected";
    } else {
      S0 = "selected";
      S1 = "";
    }
    Content += "<select style=\"width: 100%;\" size=\"1\" class=\"form-control form-select fw-bolder\" id=\"thermostat\" name=\"thermostat\">";
    Content += "<option " + S0 + " value=\"0\">No</option>";
    Content += "<option " + S1 + " value=\"1\">Yes</option>";
    Content += "</select>";
  } else {
    Content += "<input type=\"number\" step=\"" + Step + "\" min=\"" + Min + "\" max=\"" + Max + "\" class=\"form-control\" id=\"" + Name + "\" name=\"" + Name + "\" value=\"" + Value + "\">";
  }
  Content += "</form>";
  return Content;
}
//------------------------------------------------------------------------------------------------
inline String getOpMode() {
  String Temp = "";

  if (OpMode == 0) {
    Temp = "<span class=\"text-secondary\">Inactive</span>";
  } else if (OpMode == 1) {
    Temp = "<span class=\"text-warning blink\">Starting Up</span>";
  } else if (OpMode == 2) {
    if (HighBurn) {
      Temp = "<span class=\"text-success blink\">Running (high burn)</span>";
    } else {
      Temp = "<span class=\"text-success\">Running (idle)</span>";
    }
  } else if (OpMode == 3) {
    Temp = "<span class=\"text-danger blink\">Temperature Failure</span>";
  } else if (OpMode == 4) {
    Temp = "<span class=\"text-warning blink\">Shutting Down</span>";
  } else if (OpMode == 5) {
    Temp = "<span class=\"text-danger blink\">Critical Fault</span>";
  }

  return Temp;
}
//------------------------------------------------------------------------------------------------
inline String InfoLine(String Title,String Data) { // Formats a line of text in a card
  return "<div class=\"row\"><div class=\"col\" style=\"white-space: nowrap;\"><p class=\"fw-bolder text-warning mb-0\">" + Title + ":</p></div><div class=\"col\" style=\"text-align: right; white-space: nowrap;\"><p class=\"fw-bolder mb-0\">" + Data + "</p></div></div>";
}
//------------------------------------------------------------------------------------------------
inline String PageHeader() { // HTML page header with custom CSS configuration
  String Content = "";
  Content += "<!DOCTYPE html>\n";
  Content += "<html lang=\"en\">\n";
  Content += "<head>\n";
  Content +=   "<title>" + DeviceName + "</title>\n";
  Content +=   "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,shrink-to-fit=no\">\n";
  Content +=   "<meta name=\"app-version\" content=\"" + Version + "\">\n";
  Content +=   "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-GLhlTQ8iRABdZLl6O3oVMWSktQOp6b7In1Zl3/Jr59b6EGGoI1aFkw7cmDA6j6gD\" crossorigin=\"anonymous\">\n";
  Content +=   "<link rel=\"stylesheet\" href=\"https://unpkg.com/bootstrap-darkmode@0.7.0/dist/darktheme.css\">\n";
  Content +=   "<script src=\"https://code.iconify.design/2/2.0.3/iconify.min.js\"></script>\n";
  Content +=   "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.6.4/jquery.min.js\"></script>\n";
  Content +=   "<link rel=\"icon\" href=\"https://panhandleponics.com/wp-content/uploads/2025/07/cz_pellet_stove.png?v=1.1\">\n";
  Content +=   "\n<style type=\"text/css\">\n";
  Content +=   "  @-webkit-keyframes blinker {\n";
  Content +=   "    from {opacity: 1.0;}\n";
  Content +=   "    to {opacity: 0.0;}\n";
  Content +=   "  }\n";
  Content +=   "  .blink {\n";
  Content +=   "    text-decoration: blink;\n";
  Content +=   "    -webkit-animation-name: blinker;\n";
  Content +=   "    -webkit-animation-duration: 0.6s;\n";
  Content +=   "    -webkit-animation-iteration-count:infinite;\n";
  Content +=   "    -webkit-animation-timing-function:ease-in-out;\n";
  Content +=   "    -webkit-animation-direction: alternate;\n";
  Content +=   "  }\n";
  Content +=   "  a,a:hover {text-decoration: none;}\n";
  Content +=   "</style>\n";
  Content += "</head>\n";
  Content += "<body data-theme=\"dark\">\n";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String PageFooter() { // HTML page footer with custom Javascript to handle settings
  String Content = "";
  Content += "\n<div id=\"hiddenDiv\" style=\"display: none;\"></div>\n";
  Content += "\n<script type=\"text/javascript\">\n";
  Content += "function ToggleRun() {\n";
  Content += "  if (confirm('Are you sure you want to change the current run state?')) {\n";
  Content += "    jQuery('#hiddenDiv').load('./toggle-run');\n";
  Content += "  };\n";
  Content += "};\n\n";

  Content += "jQuery(document).ready(function() {\n";
  Content += "  jQuery('#submit_button').on('click',function() {\n";
  Content += "    var formData = jQuery('#modalForm').serialize();\n";
  Content += "    var dataParts = formData.split('=');\n";
  Content += "    var key = dataParts[0];\n";
  Content += "    var value = dataParts[1];\n";
  Content += "    var apiUrl = './' + key + '/' + value;\n";
  Content += "    jQuery.ajax({\n";
  Content += "      type: 'GET',\n";
  Content += "      url: apiUrl,\n";
  Content += "      success: function(response) {\n";
  Content += "        jQuery('#form-content').html('<p>Form submitted successfully</p>');\n";
  Content += "        jQuery('#dynamicModal').modal('hide');\n";
  Content += "      },\n";
  Content += "      error: function(xhr,status,error) {\n";
  Content += "        jQuery('#form-content').html('<p>An error occurred: ' + error + '</p>');\n";
  Content += "      }\n";
  Content += "    });\n";
  Content += "  });\n";
  Content += "});\n\n";

  Content += "function LoadForm(FormTitle,ID) {\n";
  Content += "  jQuery('#form-content').load('./form/' + ID,function(response,status,xhr) {\n";
  Content += "    if (status === 'success') {\n";
  Content += "      jQuery('#dynamicModalLabel').html(FormTitle);\n";
  Content += "    } else {\n";
  Content += "      jQuery('#form-content').html('Failed to load `' + FormTitle + '` form content');\n";
  Content += "    }\n";
  Content += "    jQuery('#dynamicModal').modal('show');\n";
  Content += "    jQuery('.modal-backdrop').css('opacity','0.4');\n";
  Content += "  });\n";
  Content += "};\n";
  Content += "</script>\n";
  Content += "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/js/bootstrap.min.js\" integrity=\"sha384-mQ93GR66B00ZXjt0YO5KlohRA5SY2XofN4zfuZxLkoj1gXtW8ANNCe9d5Y3eG5eD\" crossorigin=\"anonymous\"></script>\n";
  Content += "</body>\n";
  Content += "</html>\n";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String StaticData() {
  String Content = "";
  Content += InfoLine("Device Name",DeviceName);
  Content += "<button onClick=\"ToggleRun()\" class=\"btn btn-sm btn-warning fw-bolder\" type=\"button\" style=\"width: 100%; margin-top: .75em; margin-bottom: .5em;\">Start / Stop</button>";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String LiveData() {
  String Content = "";

  Content += InfoLine("Run State",getOpMode()) + "\n";
  Content += InfoLine("Uptime",Uptime) + "\n";
  Content += InfoLine("Runtime",Runtime) + "\n";
  Content += InfoLine("Countdown",Countdown) + "\n";
  if (TemperatureMode == 0) {
    Content += InfoLine("Stove Temperature",String(stoveTempF,1) + "F") + "\n";
    if (UseThermostat) Content += InfoLine("Room Temperature",String(roomTempF,1) + "F") + "\n";
  } else {
    Content += InfoLine("Stove Temp",String(stoveTempC,1) + "C") + "\n";
    if (UseThermostat) Content += InfoLine("Room Temp",String(roomTempC,1) + "C") + "\n";
  }

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String SettingsData() {
  String Content = "";
  String Temp = "";

  if (TemperatureMode == 0) {
    Temp = "Fahrenheit";
  } else {
    Temp = "Celcius";
  }
  Content += InfoLine("Temperature Mode",CreateLink(Temp,"Temperature Mode","0")) + "\n";

  if (UseThermostat) {
    Temp = "Yes";
  } else {
    Temp = "No";
  }
  Content += InfoLine("Internal Thermostat",CreateLink(Temp,"Temperature Control","1")) + "\n";

  if (UseThermostat) {
    if (TemperatureMode == 0) {
      Temp = String(targetTempF,1) + "F";
    } else {
      Temp = String(targetTempC,1) + "C";
    }
    Content += InfoLine("Thermostat Temp",CreateLink(Temp,"Thermostat Target Temperature","2")) + "\n";
  }

  if (TemperatureMode == 0) {
    Temp = String(minTempF,1) + "F";
  } else {
    Temp = String(minTempC,1) + "C";
  }
  Content += InfoLine("Stove Min Temperature",CreateLink(Temp,"Stove Minimum Temperature","3")) + "\n";

  if (TemperatureMode == 0) {
    Temp = String(maxTempF,1) + "F";
  } else {
    Temp = String(maxTempC,1) + "C";
  }
  Content += InfoLine("Stove Max Temperature",CreateLink(Temp,"Stove Maximum Temperature","4")) + "\n";

  Temp = String(StartupTimer) + " seconds";
  Content += InfoLine("Startup Time Limit",CreateLink(Temp,"Startup Time Limit (secs)","5")) + "\n";

  Temp = String(feedRateLow,1) + " seconds";
  Content += InfoLine("Idle Burn Feed Time",CreateLink(Temp,"Idle Feed Time (secs)","6")) + "\n";

  Temp = String(feedRateHigh,1) + " seconds";
  Content += InfoLine("High Burn Feed Time",CreateLink(Temp,"Idle Feed Time (secs)","7")) + "\n";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String HomePage() {
  String Content = "";
  Content += PageHeader();
  Content += CreateModal();
  Content += "<div class=\"container-fluid\" style=\"align: left;>\n";

  Content += "<div class=\"container mt-3\">\n";
  Content +=   "<div class=\"d-flex align-items-center\">\n";
  Content +=     "<img src=\"https://panhandleponics.com/wp-content/uploads/2025/07/cz_pellet_stove.png\" class=\"logo-img img-fluid me-2\" style=\"max-width: 64px; margin-bottom: 0.5em; margin-top: 0.5em;\">\n";
  Content +=     "<span class=\"fs-4 fw-bold\">CZ Pellet Stove Controller</span>\n";
  Content +=   "</div>\n";
  Content += "</div>\n\n";

  Content += "<div class=\"row\" style=\"margin-left: 0.25em; margin-right: 0.25em;\"><!-- Static Data -->\n";
  Content +=   DrawCard(StaticData(),"TopCard","",false);
  Content += "</div><!-- Static Data End -->\n\n";

  Content += "<div class=\"row\" style=\"margin-left: 0.25em; margin-right: 0.25em;\"><!-- Live Data -->\n";
  Content +=   DrawCard(LiveData(),"LiveData","ajax-livedata",true);
  Content += "</div><!-- Live Data End -->\n\n";

  Content += "<div class=\"row\" style=\"margin-left: 0.25em; margin-right: 0.25em;\"><!-- Settings Data -->\n";
  Content +=   DrawCard(SettingsData(),"SettingsData","ajax-settings",true);
  Content += "</div><!-- Settings End -->\n\n";

  Content += "</div><!-- -->\n";
  Content += PageFooter();

  return Content;
}
//------------------------------------------------------------------------------------------------
