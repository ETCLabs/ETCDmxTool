// Copyright (c) 2017 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "selectdevicedialog.h"
#include "capturedevice.h"


SelectDeviceDialog::SelectDeviceDialog(QWidget *parent ) :
    QDialog(parent),
    m_devList(Q_NULLPTR)
{
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
	m_dialog.setupUi(this);
    m_dialog.okButton->setEnabled(false);
    refreshDeviceList();
}

SelectDeviceDialog::~SelectDeviceDialog()
{
    delete m_devList;
}

ICaptureDevice *SelectDeviceDialog::selectedDevice()
{
    int row = m_dialog.listWidget->currentRow();
    if(row<0) return Q_NULLPTR;
    return m_devList->getDevice(row);
}

void SelectDeviceDialog::on_btnRefreshList_pressed()
{
    refreshDeviceList();
}

void SelectDeviceDialog::refreshDeviceList()
{
    m_dialog.listWidget->clear();
    if(m_devList) delete m_devList;

    m_devList = new CaptureDeviceList();

    QStringList devices = m_devList->deviceNames();
    m_dialog.listWidget->addItems(devices);

    if(devices.count() > 0)
    {
        m_dialog.listWidget->setCurrentItem(m_dialog.listWidget->item(0));
    }
    m_dialog.okButton->setEnabled(devices.count() > 0);

}
