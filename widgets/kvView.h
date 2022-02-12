/* vi: set sw=4 ts=4:
 *
 * Copyright (C) 2010 Christian Hohnstaedt.
 *
 * All rights reserved.
 */

#ifndef __KVVIEW_H
#define __KVVIEW_H

#include <QAbstractItemModel>
#include <QTableView>
#include <QComboBox>
#include <QItemDelegate>
#include <QLabel>

#include "lib/base.h"

class kvView;

class kvDelegate : public QItemDelegate
{
public:
	kvDelegate(QObject *parent)
		:QItemDelegate(parent)
	{
	}
    virtual void addKey(QString &) {}
};

class comboDelegate final: public kvDelegate
{
	QStringList keys;

public:
    comboDelegate(QStringList k, QObject *parent = nullptr)
			:kvDelegate(parent)
	{
		keys = k;
	}
    void addKey(QString &key) final
	{
		if (!key.isEmpty() && (keys.count() == 0 || !keys.contains(key)))
			keys << key;
	}
	QWidget *createEditor(QWidget *parent,
		const QStyleOptionViewItem &option,
        const QModelIndex &index) const final;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const final;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
            const QModelIndex &index) const final;
	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option,
        const QModelIndex &index) const final
	{
		(void)index;
		editor->setGeometry(option.rect);
	}
};

class lineDelegate final: public kvDelegate
{
	Q_OBJECT

	QLabel *infoLabel;
public:
    lineDelegate(QLabel *lbl = nullptr, QObject *parent = nullptr)
			:kvDelegate(parent)
	{
		infoLabel = lbl;
	}
	QWidget *createEditor(QWidget *parent,
		const QStyleOptionViewItem &option,
        const QModelIndex &index) const final;
    void setEditorData(QWidget *editor, const QModelIndex &index) const final;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
            const QModelIndex &index) const final;
    void updateEditorGeometry(QWidget *editor ,
		const QStyleOptionViewItem &option,
        const QModelIndex &index) const final
	{
		(void)index;
		editor->setGeometry(option.rect);
	}
signals:
	void setupLineEdit(const QString &s, QLineEdit *l) const;
};


class kvmodel final: public QAbstractTableModel
{
	QStringList items;
	QStringList header;
	int myCols;

public:
	kvmodel(QStringList &heads);
	QStringList getRow(int i);
	void addRow(const QStringList &newrow);
    Qt::ItemFlags flags(const QModelIndex &index) const final
	{
		return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
	}
	QModelIndex index(int row, int column,
            const QModelIndex &parent = QModelIndex()) const final
	{
		(void)parent;
		return createIndex(row, column, row*myCols +column);
	}
    QVariant data(const QModelIndex &index, int role) const final;
	QVariant headerData(int section, Qt::Orientation orientation,
                int role) const final;
	bool insertRows(int row, int count,
                const QModelIndex &parent = QModelIndex()) final;
	bool removeRows(int row, int count,
                const QModelIndex & parent = QModelIndex()) final;
    int rowCount(const QModelIndex &parent) const final
	{
		(void)parent;
		return items.count()/myCols;
	}
    int columnCount(const QModelIndex &parent) const final
	{
		(void)parent;
		return myCols;
	}
    bool setData(const QModelIndex &index,
                 const QVariant &value, int role) final;
	void moveRow(int oldi, int newi);
};

class kvView final: public QTableView
{
	Q_OBJECT

	QStringList keys0;
	QLabel *infoLabel;

public:
    kvView(QWidget *parent = nullptr);
    ~kvView() final;
	int rowCount()
	{
		return model()->rowCount(QModelIndex());
	}
	QStringList getRow(int i)
	{
		return static_cast<kvmodel*>(model())->getRow(i);
	}
	void addRow(const QStringList &newrow);
	void deleteAllRows()
	{
		model()->removeRows(0, rowCount(), QModelIndex());
	}
	void setInfoLabel(QLabel *lbl, int col = 1)
	{
		infoLabel = lbl;
		initLineDelegate(col);
	}
	void initLineDelegate(int col = 1);
	void setKeys(const QStringList &k, int col = 0);
	void initCols(QStringList &heads);
private slots:
	void moveRow(int logical, int oldi, int newi);
	void editorExited();
public slots:
	void addKvRow();
	void deleteCurrentRow();
};

#endif
