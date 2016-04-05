//
// Created by Tobias Hieta on 20/08/15.
//

#include "CachedRegexMatcher.h"
#include "QsLog.h"

/////////////////////////////////////////////////////////////////////////////////////////
bool CachedRegexMatcher::addMatcher(const QString& pattern, const QVariant& result)
{
  QRegExp matcher(pattern);
  if (!matcher.isValid())
  {
    QLOG_WARN() << "Could not compile pattern:" << pattern;
    return false;
  }

  m_matcherList.push_back(qMakePair(matcher, result));
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
QVariantList CachedRegexMatcher::match(const QString& input)
{
  // first we check if this match has already happened before
  if (m_matcherCache.contains(input))
    return m_matcherCache.value(input);

  QVariantList matches;

  // otherwise try to iterate our list and find a match
  for(const MatcherValuePair& matcher : m_matcherList)
  {
    QRegExp re(matcher.first);

    if (re.indexIn(input) != -1)
    {
      // found match
      QVariant returnValue = matcher.second;

      if (re.captureCount() > 0 && matcher.second.type() == QVariant::String)
      {
        QString value(matcher.second.toString());

        for (int i = 0; i < re.captureCount(); i ++)
        {
          QString argFmt = QString("%%1").arg(i + 1);
          if (value.contains(argFmt))
            value = value.arg(re.cap(i + 1));
        }
        returnValue = QVariant(value);
      }

      matches << returnValue;
    }
  }

  QLOG_DEBUG() << "No match for:" << input;

  if (!matches.isEmpty())
  {
    m_matcherCache.insert(input, matches);
    return matches;
  }

  return QVariantList();
}

/////////////////////////////////////////////////////////////////////////////////////////
void CachedRegexMatcher::clear()
{
  m_matcherCache.clear();
  m_matcherList.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
void CachedRegexMatcher::addMatches(const QVariantMap& map)
{
  for (auto key : map.keys())
    addMatcher("^" + key + "$", map.value(key));
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CachedRegexMatcher::addMatcher(const QString& pattern, CachedRegexMatcher* subMatcher)
{
  QRegExp match(pattern);
  if (!match.isValid())
  {
    QLOG_WARN() << "Could not compile pattern:" << pattern;
    return false;
  }

  m_matcherRegexList << qMakePair(pattern, subMatcher);
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
void CachedRegexMatcher::addRegexMatches(const QVariantMap& map)
{
  for (auto key : map.keys())
  {
    if (map.value(key).type() != QVariant::Map)
      continue;

    auto seq2 = map.value(key).toMap();
    auto crm = new CachedRegexMatcher(seq2, this);
    addMatcher("^" + key + "$", crm);
    QLOG_DEBUG() << "Added sequence starting with:" << key;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
CachedRegexMatcher* CachedRegexMatcher::matchRegex(const QString& input)
{
  if (m_matcherRegexCache.contains(input))
  {
    return m_matcherRegexCache.value(input);
  }
  else
  {
    for (auto match : m_matcherRegexList)
    {
      if (match.first.indexIn(input) != -1)
      {
        m_matcherRegexCache.insert(input, match.second);
        return match.second;
      }
    }
  }

  return nullptr;
}


