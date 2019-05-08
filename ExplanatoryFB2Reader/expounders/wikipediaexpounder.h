#ifndef WIKIPEDIAEXPOUNDER_H
#define WIKIPEDIAEXPOUNDER_H

#include "expounder.h"

class WikipediaExpounder : public Expounder
{
public:
    WikipediaExpounder(const QString &language);

    void findExplanation(const QString &input) const override;

};

#endif // WIKIPEDIAEXPOUNDER_H
