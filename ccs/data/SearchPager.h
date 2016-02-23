#ifndef SEARCHPAGER_H
#define SEARCHPAGER_H

#include <QVector>

struct SearchPagerPageInformation {
    quint64 page_start_sequence;
    quint64 page_end_sequence;
    SearchPagerPageInformation();
    SearchPagerPageInformation(const SearchPagerPageInformation& _page_information);
    SearchPagerPageInformation(quint64 _page_start_sequence,
                               quint64 _page_end_sequence);
};


class SearchPager {

public:
    SearchPager();
    virtual ~SearchPager();

    quint32 getNumberOfPage();
    bool getCurrentPage(SearchPagerPageInformation& next_page);
    bool getNextPage(SearchPagerPageInformation& next_page);
    bool getPreviousPage(SearchPagerPageInformation& prev_page);
    void addNewPage(quint64 _page_start_sequence,
                     quint64 _page_end_sequence);
    void clear();
private:
    qint32 current_page;
    QVector<SearchPagerPageInformation> page_vector;
};

#endif // SEARCHPAGER_H
