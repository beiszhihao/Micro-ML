const { app, BrowserWindow ,Notification, ipcMain} = require('electron')

var win
app.on('ready',()=> {
    win = new BrowserWindow({
        width: 1200,
        height: 550,
        frame: false,
        show: false,
        webPreferences: {
            nodeIntegration:true,
            contextIsolation: false
        }
  })
  win.setMenu(null)
  win.loadFile('src/view/index.html')
  win.once('ready-to-show', () => {
    win.show()
  })
})

ipcMain.on('window-close', () => {
   win.close()
})

ipcMain.on('window-max', function () {
    if (win.isMaximized()) {
        win.restore();
    } else {
        win.maximize();
    }
})

ipcMain.on('window-min', function () {
    win.minimize();
})

ipcMain.on('openFile', function () {
    const { dialog } = require('electron');
    dialog.showOpenDialog(win, {
        properties: ['openFile'], 
        filters: [
            { name: 'Images', extensions: ['jpg', 'jpeg'] }
          ]
      }).then(result => {
        win.webContents.send('set_img_file_path', result.filePaths);
      })
    
})

ipcMain.on('openModel', function () {
    const { dialog } = require('electron');
    dialog.showOpenDialog(win, {
        properties: ['openFile'], 
        filters: [
            { name: 'Model', extensions: ['tflite'] }
          ]
      }).then(result => {
        win.webContents.send('set_model_file_path', result.filePaths);
      })
    
})