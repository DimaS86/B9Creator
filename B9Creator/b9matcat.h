#ifndef B9MATCAT_H
#define B9MATCAT_H

#include <QObject>
#include <QDataStream>

#define XYCOUNT 3
#define ZCOUNT 16
#define TCOUNT 2

class B9MatCatItem
{
public:
    B9MatCatItem();

    QString getMaterialLabel(){return m_sMaterialLabel;}
    QString getMaterialDescription(){return m_sMaterialDescription;}
    void setMaterialLabel(QString sLabel){m_sMaterialLabel = sLabel;}
    void setMaterialDescription(QString sLabel){m_sMaterialDescription = sLabel;}

    double getTbase(int iXY, int iZ){return m_aTimes[iXY][iZ][0];}
    double getTover(int iXY, int iZ){return m_aTimes[iXY][iZ][1];}
    void setTbase(int iXY, int iZ, double dSeconds){m_aTimes[iXY][iZ][0]=dSeconds;}
    void setTover(int iXY, int iZ, double dSeconds){m_aTimes[iXY][iZ][1]=dSeconds;}

private:
    void initDefaults();
    double m_aTimes [XYCOUNT][ZCOUNT][TCOUNT]; // Cure times in seconds
    QString m_sMaterialLabel, m_sMaterialDescription;
};

///////////////////////////////////////////////////////////////////////////
class B9MatCat : public QObject
{
    Q_OBJECT
public:
    explicit B9MatCat(QObject *parent = 0);

    int getMaterialCount(){return m_Materials.count();}
    int getXYCount(){return XYCOUNT;}
    int getZCount(){return ZCOUNT;}

    QString getMaterialLabel(int index){return m_Materials.at(index)->getMaterialLabel();}
    QString getMaterialDescription(int index){return m_Materials.at(index)->getMaterialDescription();}

    QString getTbase(int iMat, int iXY, int iZ){return QString::number(m_Materials.at(iMat)->getTbase(iXY, iZ));}
    QString getTover(int iMat, int iXY, int iZ){return QString::number(m_Materials.at(iMat)->getTover(iXY, iZ));}

    void setTbase(int iMat, int iXY, int iZ, double dT){m_Materials.at(iMat)->setTbase(iXY, iZ, dT);}
    void setTover(int iMat, int iXY, int iZ, double dT){m_Materials.at(iMat)->setTover(iXY, iZ, dT);}

    QString getCurTbaseAtZ(int iZ){return QString::number(m_Materials.at(m_iCurMatIndex)->getTbase(m_iCurXYIndex, iZ));}
    QString getCurToverAtZ(int iZ){return QString::number(m_Materials.at(m_iCurMatIndex)->getTover(m_iCurXYIndex, iZ));}

    QString getModelName(){return m_sModelName;}

    QString getXYLabel(int iXY);
    double getXYinMM(int iXY);
    QString getZLabel(int iZ);
    double getZinMM(int iZ);

    void deleteMaterial(int iMat){m_Materials.removeAt(iMat);}

    void setCurMatIndex(int iM){m_iCurMatIndex=iM;}
    void setCurXYIndex(int iXY){m_iCurXYIndex=iXY;}
    void setCurZIndex(int iZ){m_iCurZIndex=iZ;}
    int getCurMatIndex(){return m_iCurMatIndex;}
    int getCurXYIndex(){return m_iCurXYIndex;}
    int getCurZIndex(){return m_iCurZIndex;}

signals:

public slots:
    void addMaterial(QString sName, QString sDescription);
    void addDupMat(QString sName, QString sDescription,int iOriginIndex);
    void clear();
    bool load(QString sModelName);
    bool save();

private:
    QList<B9MatCatItem*> m_Materials;
    QString m_sModelName;
    void streamOut(QDataStream* pOut);
    void streamIn(QDataStream* pIn);

    int m_iCurMatIndex;
    int m_iCurXYIndex;
    int m_iCurZIndex;

};

#endif // B9MATCAT_H
