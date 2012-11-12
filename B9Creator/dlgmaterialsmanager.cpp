#include "dlgmaterialsmanager.h"
#include "ui_dlgmaterialsmanager.h"
#include <QSpinBox>
#include <QMessageBox>
#include <QInputDialog>


MaterialsTableItemDelegate::MaterialsTableItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

MaterialsTableItemDelegate::~MaterialsTableItemDelegate()
{
}

QWidget* MaterialsTableItemDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if(index.column() > 2 || index.column() < 0)
        return QStyledItemDelegate::createEditor(parent, option, index);

    // Create the spinBox
    QDoubleSpinBox *sb = new QDoubleSpinBox(parent);
    sb->setDecimals(3);
    sb->setMaximum(120);
    sb->setMinimum(0);
    sb->setSingleStep(0.5);
    return sb;
}
void MaterialsTableItemDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
    if(QComboBox *cb = qobject_cast<QComboBox *>(editor))
        // save the current text of the combo box as the current value of the item
        model->setData(index, cb->currentText(), Qt::EditRole);
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}


DlgMaterialsManager::DlgMaterialsManager(B9MatCat *pCatalog, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgMaterialsManager)
{
    ui->setupUi(this);
    m_pCatalog = pCatalog;

    MaterialsTableItemDelegate *mtid = new MaterialsTableItemDelegate(0);
    ui->tableWidget->setItemDelegate(mtid);
    setUp();
    fillData(0,0);
}

DlgMaterialsManager::~DlgMaterialsManager()
{
    delete ui;
}

void DlgMaterialsManager::setUp()
{
    if(m_pCatalog->getMaterialCount()<1){
        m_pCatalog->setCurMatIndex(-1);
        ui->groupBox_ExposureSettings->setEnabled(false);
        ui->pushButtonDuplicate->setEnabled(false);
        ui->pushButtonDelete->setEnabled(false);
    }
    else
    {
        ui->groupBox_ExposureSettings->setEnabled(true);
        ui->pushButtonDuplicate->setEnabled(true);
        ui->pushButtonDelete->setEnabled(true);
    }

    setWindowTitle("Materials Catalog: "+m_pCatalog->getModelName()+".b9m");
    ui->tableWidget->setRowCount(m_pCatalog->getZCount());
    ui->tableWidget->setColumnCount(2);

    // Fill Rows with Z labels
    for(int r=0; r<m_pCatalog->getZCount();r++){
        ui->tableWidget->setVerticalHeaderItem(r,new QTableWidgetItem(m_pCatalog->getZLabel(r)));
        ui->tableWidget->setItem(r,0,new QTableWidgetItem());
        ui->tableWidget->setItem(r,1,new QTableWidgetItem());
        ui->tableWidget->item(r,0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(r,1)->setTextAlignment(Qt::AlignCenter);
    }

    // Fill Materials Combo Box
    ui->comboBoxMaterial->clear();
    for(int i=0; i<m_pCatalog->getMaterialCount();i++){
        ui->comboBoxMaterial->addItem(m_pCatalog->getMaterialLabel(i));
    }

    // Fill XY Select Combo Box
    ui->comboBoxXY->clear();
    for(int i=0; i<m_pCatalog->getXYCount();i++){
        ui->comboBoxXY->addItem(m_pCatalog->getXYLabel(i));
    }
}


void DlgMaterialsManager::fillData(int iMatIndex, int iXYIndex)
{
    m_pCatalog->setCurMatIndex(iMatIndex);
    m_pCatalog->setCurXYIndex(iXYIndex);
    m_pCatalog->setCurZIndex(0);

    ui->comboBoxMaterial->setCurrentIndex(m_pCatalog->getCurMatIndex());
    ui->comboBoxXY->setCurrentIndex(m_pCatalog->getCurXYIndex());

    // Fill table with current Material and current XY
    for(int r=0; r<ui->tableWidget->rowCount();r++){
        ui->tableWidget->setItem(r,0,new QTableWidgetItem());
        ui->tableWidget->setItem(r,1,new QTableWidgetItem());
        ui->tableWidget->item(r,0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(r,1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->item(r,0)->setText(m_pCatalog->getTbase(m_pCatalog->getCurMatIndex(),m_pCatalog->getCurXYIndex(),r));
        ui->tableWidget->item(r,1)->setText(m_pCatalog->getTover(m_pCatalog->getCurMatIndex(),m_pCatalog->getCurXYIndex(),r));
    }
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resizeColumnsToContents();
}

void DlgMaterialsManager::stuffData()
{
    for(int r=0; r<ui->tableWidget->rowCount();r++){
        m_pCatalog->setTbase(m_pCatalog->getCurMatIndex(),m_pCatalog->getCurXYIndex(),r,ui->tableWidget->item(r,0)->text().toDouble());
        m_pCatalog->setTover(m_pCatalog->getCurMatIndex(),m_pCatalog->getCurXYIndex(),r,ui->tableWidget->item(r,1)->text().toDouble());
    }
}

void DlgMaterialsManager::on_comboBoxMaterial_currentIndexChanged(int index)
{
    if(index<0)return;
    if(m_pCatalog->getMaterialCount()<1){
        ui->lineEditDescription->setText("Empty Catalog");
        return;
    }
    ui->lineEditDescription->setText(m_pCatalog->getMaterialDescription(index));
    fillData(index, m_pCatalog->getCurXYIndex());
}

void DlgMaterialsManager::on_comboBoxXY_currentIndexChanged(int index)
{
    if(index<0)return;
    stuffData();
    fillData(m_pCatalog->getCurMatIndex(), index);
}


void DlgMaterialsManager::on_pushButtonDelete_clicked()
{
    if(QMessageBox::warning(this, tr("Delete Material"),tr("Are you sure you wish to delete this material from the Catalog?"),QMessageBox::Yes|QMessageBox::No)==QMessageBox::No)return;
    m_pCatalog->deleteMaterial(m_pCatalog->getCurMatIndex());
    m_pCatalog->setCurMatIndex(0);
    m_pCatalog->setCurXYIndex(0);
    m_pCatalog->setCurZIndex(0);
    setUp();
}

void DlgMaterialsManager::on_pushButtonAdd_clicked()
{
    bool ok;
    QString sMatID = QInputDialog::getText(this, tr("Material ID"),
                                         tr("Enter a short unique Identifier for this Material:"), QLineEdit::Normal,
                                         "MaterialID", &ok);
    if (!(ok && !sMatID.isEmpty())) return;
    if(ui->comboBoxMaterial->findText(sMatID,Qt::MatchFixedString)!=-1){
        QMessageBox::warning(this,tr("Duplicate Material ID!"),sMatID+" already exists in the Catalog.  Please enter a unique ID.",QMessageBox::Ok);
        return;
    }

    QString sDecrip = QInputDialog::getText(this, tr("Material ID"),
                                             tr("Enter a short, unique Identifier for this Material:"), QLineEdit::Normal,
                                             "Material Description", &ok);
        if (!ok) return;

    stuffData();
    m_pCatalog->addMaterial(sMatID, sDecrip);

    setUp();
    m_pCatalog->setCurMatIndex(m_pCatalog->getMaterialCount()-1);
    ui->comboBoxMaterial->setCurrentIndex(m_pCatalog->getCurMatIndex());
}


void DlgMaterialsManager::on_buttonBoxSaveCancel_accepted()
{
    stuffData();
    m_pCatalog->save();
}

void DlgMaterialsManager::on_buttonBoxSaveCancel_rejected()
{
    m_pCatalog->load(m_pCatalog->getModelName());
}

void DlgMaterialsManager::on_pushButtonDuplicate_clicked()
{
    stuffData();

    QString sID = m_pCatalog->getMaterialLabel(ui->comboBoxMaterial->currentIndex())+"-Duplicate";
    bool ok;
    QString sMatID = QInputDialog::getText(this, tr("Material ID"),
                                         tr("Enter a short, unique Identifier for this Material:"), QLineEdit::Normal,
                                         sID, &ok);
    if (!(ok && !sMatID.isEmpty())) return;
    if(ui->comboBoxMaterial->findText(sMatID,Qt::MatchFixedString)!=-1){
        QMessageBox::warning(this,tr("Duplicate Material ID!"),sMatID+" already exists in the Catalog.  Please enter a unique ID.",QMessageBox::Ok);
        return;
    }

    QString sDecrip = QInputDialog::getText(this, tr("Material ID"),
                                             tr("Enter a short unique Identifier for this Material:"), QLineEdit::Normal,
                                             m_pCatalog->getMaterialDescription(ui->comboBoxMaterial->currentIndex()), &ok);
        if (!ok) return;


    m_pCatalog->addDupMat(sMatID, sDecrip, ui->comboBoxMaterial->currentIndex());

    setUp();
    m_pCatalog->setCurMatIndex(m_pCatalog->getMaterialCount()-1);
    ui->comboBoxMaterial->setCurrentIndex(m_pCatalog->getCurMatIndex());

}

