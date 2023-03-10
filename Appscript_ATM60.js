var sheet_id="1tnWOXcl-w84Nvlz50ICdKTSFsi2tv4ss9kH63Q_Z9wY";
var sheet_name="BT20ECE060_Kumar_Mridul";

var ss = SpreadsheetApp.openById(sheet_id);
var sheet = ss.getSheetByName(sheet_name);

function doGet(e){

  var request = e.parameter.request;
  if (request == "read"){
    var row = Number(e.parameter.row);
    var column = e.parameter.column;
    var cell = column + row.toString();
    return ContentService.createTextOutput(sheet.getRange(cell).getValue());
  }

  else if (request == "write"){
    var username = e.parameter.username;
    var password = e.parameter.password;
    var id = SpreadsheetApp.getActiveSheet().getLastRow();    //new_id = row count of last filled row
    sheet.appendRow([id,username,password,15000]);            //appending new row of having informations of new user 
    var data = sheet.getDataRange().getValues();
    data = data[id][0];
    return ContentService.createTextOutput(data);
  }
  
  else if (request == "update"){
    var row = Number(e.parameter.row);
    var column = e.parameter.column;
    var value = e.parameter.value;
    var cell = column + row.toString();
    //updating the current balance amount
    SpreadsheetApp.getActiveSheet().getRange(cell).setValue(value);
  }

}