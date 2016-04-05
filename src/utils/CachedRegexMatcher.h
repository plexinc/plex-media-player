//
// Created by Tobias Hieta on 20/08/15.
//

#ifndef KONVERGO_CACHEDREGEXMATCHER_H
#define KONVERGO_CACHEDREGEXMATCHER_H

#include <QRegExp>
#include <QVariant>
#include <QString>
#include <QHash>

class CachedRegexMatcher;

typedef QPair<QRegExp, QVariant> MatcherValuePair;
typedef QList<MatcherValuePair> MatcherValueList;

typedef QPair<QRegExp, CachedRegexMatcher*> MatcherValuePairRegex;
typedef QList<MatcherValuePairRegex> MatcherValueListRegex;

class CachedRegexMatcher : public QObject
{
public:
  explicit CachedRegexMatcher(QObject* parent = nullptr) : QObject(parent) {}
  explicit CachedRegexMatcher(const QVariantMap& matches, QObject* parent = nullptr) : QObject(parent)
  {
    addMatches(matches);
  }

  bool addMatcher(const QString& pattern, const QVariant& result);
  void addMatches(const QVariantMap& map);

  bool addMatcher(const QString& pattern, CachedRegexMatcher* subMatcher);
  void addRegexMatches(const QVariantMap& map);

  QVariantList match(const QString& input);
  CachedRegexMatcher* matchRegex(const QString& input);

  void clear();

private:
  MatcherValueList m_matcherList;
  QHash<QString, QVariantList> m_matcherCache;

  MatcherValueListRegex m_matcherRegexList;
  QHash<QString, CachedRegexMatcher*> m_matcherRegexCache;
};

#endif //KONVERGO_CACHEDREGEXMATCHER_H
