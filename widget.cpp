#include "widget.h"
#include "ui_widget.h"
#include "QTextCodec"
Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::Window);
    this->setWindowTitle(tr("随机点名系统 "));//设置窗口名
    this->setFixedSize(500,600);        //设置窗口固定大小

    QTime time = QTime::currentTime();  //获得当前时间
    qsrand(time.msec()+time.second()*10000);    //用时间初始化随机数种子

    mainLayout = new QGridLayout(this); //创建格子布局

    nameDisplayLabel = new QLabel(tr("随机点名系统")); //新建QLabel
    nameDisplayLabel->setAlignment(Qt::AlignCenter);//QLabel控件居中显示
    nameDisplayLabel->setFont(QFont("华文行楷", 50));//设置字体格式
    switchButton = new QPushButton(tr("开始"));//新建开关按钮
    importButton = new QPushButton(tr("导入"));//新建导入按钮

    switchTime = new QTimer;//新建一个定时器

    mainLayout->addWidget(nameDisplayLabel, 0,0,1,2);//添加nameDisplayLabel控件到mainlayout的第0行0列，并占1行2列
    mainLayout->addWidget(importButton, 1,0);//添加importButton控件到mainlayout的第1行0列
    mainLayout->addWidget(switchButton, 1,1);//添加switchButton控件到mainlayout的第1行1列

    this->connect(importButton, SIGNAL(clicked()), this, SLOT(importButtonClicked()));
    this->connect(switchButton, SIGNAL(clicked()), this, SLOT(switchButtonClicked()));
    this->connect(switchTime, SIGNAL(timeout()), this, SLOT(switchTimeOut()));
}

Widget::~Widget()
{
}

void Widget::importButtonClicked()
{
    if (strListImport.isEmpty())//数据导入为空则导入，否则给出提示信息
        importData();
    else
    {
        //提示信息
        switch(QMessageBox::question(this, tr("提示："), tr("本次导入会覆盖上一次数据，是否继续？"), QMessageBox::No|QMessageBox::No, QMessageBox::Yes))
        {
        case QMessageBox::Yes:
            strListImport.clear();//选择yes则清空数据表，计数器清0， 再导入数据信息
            count = 0;
            importData();
            break;
        default://默认不尽兴任何操作
            break;
        }
    }
}

void Widget::switchButtonClicked()
{
    if (count == 0)//若计数器为0，给出提示信息
        QMessageBox::information(this, tr("提示："), tr("您还没有导入数据！"));
    else
    {//开关按钮标志位，每按一次取反一次
        switchButtonFlg = !switchButtonFlg;
        if (switchButtonFlg)
        {
            switchButton->setText(tr("停止"));
            switchTime->start(50);
            qDebug()<<strListImport;
        }
        else
        {
            switchButton->setText(tr("开始"));
            switchTime->stop();
            strListImport.removeAt(test);//移除已被点名的人
            count--;
            qDebug()<<strListImport;
        }
    }
}

void Widget::switchTimeOut()//定时器溢出处理函数
{
    test = qrand()%count;//初始化随机数
    nameDisplayLabel->setText(strListImport.at(test));
    qDebug() << "随机数：" << test << " count: " << count;
}

void Widget::importData()//数据导入函数
{//获取文件
    QString fileName = QFileDialog::getOpenFileName(this,"选择花名册",qApp->applicationDirPath()+"/花名册" ,"files(*.txt)");
    if(!fileName.isEmpty())
    {
        QTextCodec *codec = QTextCodec::codecForName("utf8");
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text))//以只读方式打开文本信息
        {
            while(!file.atEnd())
            {//一行一行的读取数据
                QByteArray  temp = file.readLine();
                if (!temp.isEmpty())//数据把不为空加入QStringList表中，且计数器加1
                {
                    QString str = codec->toUnicode(temp);
                    strListImport.append(str);
                    count++;
                }
            }
            QMessageBox::information(this, tr("提示："), tr("您成功导入 %1 个名字！").arg(count));
        }
    }
    else
        QMessageBox::information(this, tr("提示："), tr("导入失败，请检查格式是否正确！"));
}
