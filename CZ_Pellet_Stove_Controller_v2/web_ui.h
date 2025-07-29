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
  Content +=         "<button type=\"button\" class=\"btn btn-sm btn-success\" id=\"submit_button\">&nbsp;Save&nbsp;</button>";
  Content +=       "</div>";
  Content +=     "</div>";
  Content +=   "</div>";
  Content += "</div>\n\n";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String DrawCard(String Body,String AjaxID,String Query,bool DoAjax) { // Plots a Bootstrap card
  String Content = "";
  if (DoAjax) Content += AjaxRefreshJS(AjaxID,Query,"4000");
  Content += "<div class=\"card\" style=\"width: 100%;margin-top: 0.5em;margin-bottom: 0.5em;margin-left: 0.5em;margin-right: 0.5em;\">";
  Content +=   "<div class=\"card-body\">";
  Content +=     "<div id=\"" + AjaxID + "\">";
  Content +=      Body;
  Content +=     "</div>";
  Content +=   "</div>";
  Content += "</div>";

  return Content;
}
//------------------------------------------------------------------------------------------------
inline String get_Form(byte WhichOne) { // Dynamically creates the form for the specified setting
  String Content = "";
  String Label,Value,Step,Min,Max;
/*
  if (WhichOne == 0) {
    Label = "Operation Mode";
  } else if (WhichOne == 1) {
    Label = "0.0 to 260.0 (C)";
    Step = ".1"; Min = "0"; Max = "260"; Value = String(TargetTemp,1);
  } else if (WhichOne == 2) {
    Label = "10% to 100%";
    Step = "1"; Min = "10"; Max = "100"; Value = String(StartupPercent);
  } else if (WhichOne == 3) {
    Label = "10% to 100%";
    Step = "1"; Min = "10"; Max = "100"; Value = String(FallBackPercent);
  } else if (WhichOne == 4) {
    Label = "1% to 100%";
    Step = "1"; Min = "1"; Max = "100"; Value = String(AdjustRate);
  } else if (WhichOne == 5) {
    Label = "0.1 to 5.0 (C)";
    Step = ".1"; Min = ".1"; Max = "5"; Value = String(Deviation,1);
  } else if (WhichOne == 6) {
    Label = "1 to 1000";
    Step = "1"; Min = "1"; Max = "1000"; Value = String(ChangeWait);
  } else if (WhichOne == 7) {
    Label = "1 to 1000";
    Step = "1"; Min = "1"; Max = "1000"; Value = String(RestPeriod);
  }

  Content += "<form id=\"modalForm\" onsubmit=\"return false;\">";
  Content +=   "<label for=\"data_" + String(WhichOne) + "\" class=\"form-label\">" + Label + "</label>";
  if (WhichOne == 0) {
    String S0,S1;
    if (OpMode == 0) {
      S0 = "selected";
      S1 = "";
    } else {
      S0 = "";
      S1 = "selected";
    }
    Content += "<select style=\"width: 100%;\" size=\"1\" class=\"form-control form-select fw-bolder\" id=\"data_0\" name=\"data_0\">";
    Content += "<option " + S0 + " value=\"0\">Constant Power</option>";
    Content += "<option " + S1 + " value=\"1\">Constant Temperature</option>";
    Content += "</select>";
  } else {
    Content +=   "<input type=\"number\" step=\"" + Step + "\" min=\"" + Min + "\" max=\"" + Max + "\" class=\"form-control\" id=\"data_" + String(WhichOne) + "\" name=\"data_" + String(WhichOne) + "\" value=\"" + Value + "\">";
  }
  Content += "</form>";
*/
  return Content;
}
//------------------------------------------------------------------------------------------------
inline String InfoLine(String Title,String Data) { // Formats a line of text in a card
  return "<div class=\"row\"><div class=\"col-5\" style=\"padding: 2px;\"><p class=\"fw-bolder text-success mb-0\">" + Title + ":</p></div><div class=\"col-7\" style=\"padding: 2px;\"><p class=\"fw-bolder mb-0\" style=\"text-align: right;\">" + Data + "</p></div></div>";
}
//------------------------------------------------------------------------------------------------
inline String PageHeader() { // HTML page header with custom CSS configuration
  String Content = "";
  //Content += "<!-- " + AppIcon + " -->";
  Content += "<!DOCTYPE html>\n";
  Content += "<html lang=\"en\">\n";
  Content += "<head>\n";
  Content +=   "<title>" + DeviceName + "</title>\n";
  Content +=   "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,shrink-to-fit=no\">\n";
  Content +=   "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-GLhlTQ8iRABdZLl6O3oVMWSktQOp6b7In1Zl3/Jr59b6EGGoI1aFkw7cmDA6j6gD\" crossorigin=\"anonymous\">\n";
  Content +=   "<link rel=\"stylesheet\" href=\"https://unpkg.com/bootstrap-darkmode@0.7.0/dist/darktheme.css\">\n";
  Content +=   "<script src=\"https://code.iconify.design/2/2.0.3/iconify.min.js\"></script>\n";
  Content +=   "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.6.4/jquery.min.js\"></script>\n";
  Content +=   "<link rel=\"icon\" href=\"https://panhandleponics.com/wp-content/uploads/2024/12/boilermaker.png?v=1.1\">\n";
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
  Content += "<div id=\"hiddenDiv\" style=\"display: none;\"></div>\n";
  Content += "\n<script type=\"text/javascript\">\n";
  Content += "function ToggleRun() {\n";
  Content += "  if (confirm('Are you sure you want to change the current run state?')) {\n";
  Content += "    jQuery('#hiddenDiv').load('./toggle-run');\n";
  Content += "  };\n";
  Content += "};\n\n";

  Content += "jQuery(document).ready(function() {\n";
  Content += "  jQuery('#submit_button').on('click',function() {\n";
  Content += "    var formData = jQuery('#modalForm').serialize();\n";
  Content += "    jQuery.ajax({\n";
  Content += "      type: 'GET',\n";
  Content += "      url: './',\n";
  Content += "      data: formData,\n";
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


//------------------------------------------------------------------------------------------------
inline String HomePage() {
  String Content = "";
  Content += PageHeader();
  Content += CreateModal();
  Content += "<div class=\"container-fluid\" style=\"align: left;\">\n";


  Content += "</div>\n";
  Content += PageFooter();

  return Content;
}
//------------------------------------------------------------------------------------------------
