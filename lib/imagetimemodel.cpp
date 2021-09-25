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

#include "imagetimemodel.h"
#include "imagestorage.h"

ImageTimeModel::ImageTimeModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_hours(24)
{
    QList<ImageInfo> list = ImageStorage::instance()->images();

    for (const ImageInfo& ii : list) {
        m_categorizer.addImage(ii);
    }
}

QHash<int, QByteArray> ImageTimeModel::roleNames() const
{
    auto hash = QAbstractItemModel::roleNames();
    hash.insert(FilesRole, "files");

    return hash;
}

QVariant ImageTimeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    QString key = m_images.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return key;

        case FilesRole: {
            return m_categorizer.imagesForHours(m_hours, key);
        }
    }

    return QVariant();

}

int ImageTimeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_images.size();
}

int ImageTimeModel::hours() const
{
    return m_hours;
}

void ImageTimeModel::setHours(int hours)
{
    beginResetModel();
    m_hours = hours;
    m_images = m_categorizer.groupByHours(m_hours);
    endResetModel();
}
