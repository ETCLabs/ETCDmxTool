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

#ifndef DISSECTORS_H
#define DISSECTORS_H

#include <QObject>
#include <QTreeWidget>
#include <QList>
#include <QLibrary>
#include "packetbuffer.h"
#include "dissectors/dissectorplugin.h"

class dissectors : public QObject
{
    Q_OBJECT
public:
    explicit dissectors(QObject *parent = nullptr);

    DissectorPlugin *getDissector(const Packet &p);

    struct s_DissectorList
    {
        DissectorPlugin* dissector;
        bool isEnabled() { return m_enabled; }
        void setEnabled(bool enabled = true) {
            m_enabled = enabled;
        }

        explicit s_DissectorList(DissectorPlugin* d)
        {
            m_enabled = true;
            dissector = d;
        }

    private:
        bool m_enabled;
    };
   auto getDissectorList() { return m_dissectors; }

signals:

public slots:

private:
    QList<s_DissectorList*> m_dissectors;

#if defined (Q_OS_WIN)
    const QString disectorPatternMatch = "dissectorplugin*.dll";
#elif defined(Q_OS_LINUX)
    const QString disectorPatternMatch = "libdissectorplugin*.so";
#endif
};

#endif // DISSECTORS_H
