function doPost(e) {
  var publishedAt = new Date(e.parameter.published_at);
  var dataArray = [];

  try {
    dataArray = JSON.parse(e.parameter.data);
  } catch (error) {
    console.error("Error parsing JSON:", error);
    // You might want to do something with the error, like sending a notification.
  }

  var spreadsheet = SpreadsheetApp.openById('1Qc1ybVogc6IO5egKVjaZ1nuiXp4zHfKNJJ7quORJ_ck');
  if (!spreadsheet) {
    console.error("Spreadsheet not found");
    return;
  }

  // Create or get sheets for each dataArray
  var sheet1 = getOrCreateSheet(spreadsheet, 'WindSpeed');
  var sheet2 = getOrCreateSheet(spreadsheet, 'X Force (Strain1)');
  var sheet3 = getOrCreateSheet(spreadsheet, 'Y Force (Strain2)');

  // Split dataArray into three arrays
  var dataArray1 = dataArray.slice(0, 30);
  var dataArray2 = dataArray.slice(30, 60);
  var dataArray3 = dataArray.slice(60);

  // Create rows with the first, next, and last 32 elements
  var row1 = [e.parameter.coreid, Utilities.formatDate(publishedAt, 'GMT', 'yyyy-MM-dd HH:mm:ss')];
  row1 = row1.concat(dataArray1.map(value => (value * 8.75) / 100));

  var row2 = [Utilities.formatDate(publishedAt, 'GMT', 'yyyy-MM-dd HH:mm:ss')];
  row2 = row2.concat(dataArray2.map(value => value / 10));

  var row3 = [Utilities.formatDate(publishedAt, 'GMT', 'yyyy-MM-dd HH:mm:ss')];
  row3 = row3.concat(dataArray3.map(value => value / 10));

  // Append rows to the respective sheets
  sheet1.appendRow(row1);
  sheet2.appendRow(row2);
  sheet3.appendRow(row3);

  // Auto-resize columns after appending rows
  autoResizeColumns(sheet1);
  autoResizeColumns(sheet2);
  autoResizeColumns(sheet3);

  var result = { ok: true };

  return ContentService.createTextOutput(JSON.stringify(result))
    .setMimeType(ContentService.MimeType.JSON);
}

// Helper function to get or create a sheet by name
function getOrCreateSheet(spreadsheet, sheetName) {
  var sheet = spreadsheet.getSheetByName(sheetName);
  if (!sheet) {
    sheet = spreadsheet.insertSheet(sheetName);
  }
  return sheet;
}

// Helper function to auto-resize columns in a sheet
function autoResizeColumns(sheet) {
  var maxColumns = sheet.getLastColumn();
  for (var i = 1; i <= maxColumns; i++) {
    sheet.autoResizeColumn(i);
  }
}
