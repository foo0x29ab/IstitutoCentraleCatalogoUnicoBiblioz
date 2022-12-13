/*
 * Copyright (C) 2014  Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "sortmodel.h"
#include <QDebug>

using namespace Jungle;

SortModel::SortModel(QObject* parent): QSortFilterProxyModel(parent)
{
    setSortLocaleAware(true);
    sort(0);
    m_selectionModel = new QItemSelectionModel(this);
}

SortModel::~SortModel()
{
}

QByteArray SortModel::sortRoleName() const
{
    int role = sortRole();
    return roleNames().value(role);
}

void SortModel::setSortRoleName(const QByteArray& name)
{
    if (!sourceModel()) {
        m_sortRoleName = name;
        return;
    }

    const QHash<int, QByteArray> roles = sourceModel()->roleNames();
    for (auto it = roles.begin(); it != roles.end(); it++) {
        if (it.value() == name) {
            setSortRole(it.key());
            return;
        }
    }
    qDebug() << "Sort role" << name << "not found";
}

QHash<int, QByteArray> SortModel::roleNames() const
{
    QHash<int, QByteArray> hash = sourceModel()->roleNames();
    hash.insert(Role::SelectedRole, "selected");
    
    return hash;
}


QVariant SortModel::data(const QModelIndex& index, int role) const
{
    if( !index.isValid()) {
        return QVariant();
    }
    
    if( role == Role::SelectedRole) {
        return m_selectionModel->isSelected(index);
    }
    
    return QSortFilterProxyModel::data(index, role);
}

void SortModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);

    if (!m_sortRoleName.isEmpty()) {
        setSortRoleName(m_sortRoleName);
        m_sortRoleName.clear();
    }
}

void SortModel::setSelected(int indexValue)
{
    if( indexValue < 0)
        return;

    QModelIndex index = QSortFilterProxyModel::index( indexValue, 0);
    m_selectionModel->select( index, QItemSelectionModel::Select );
    emit dataChanged( index, index);
}

void SortModel::toggleSelected(int indexValue )
{
    if( indexValue < 0)
        return;
    
    QModelIndex index = QSortFilterProxyModel::index( indexValue, 0);
    m_selectionModel->select( index, QItemSelectionModel::Toggle );
    emit dataChanged( index, index);
}

void SortModel::clearSelections()
{
    if(m_selectionModel->hasSelection()) {
        QModelIndexList selectedIndex = m_selectionModel->selectedIndexes();
        m_selectionModel->clear();
        foreach(QModelIndex indexValue, selectedIndex) {
            emit dataChanged( indexValue, indexValue);
        }
    }
}
