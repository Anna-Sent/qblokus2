#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>
#include <QString>

using namespace std;

class Tile
{
private:
    int _score;

public:
    vector<vector<char> > data;
    Tile(string type);
    string getAsText() const;
    QString getAsQString() const { return QString(getAsText().c_str()); }
    int height() const { return data.size(); }
    int width() const { return data.size() > 0 ? data[0].size() : 0; }
    int score() const { return _score; }
    void reflect();
    void rotate();
};

ostream & operator<<(ostream &, const Tile &);

#endif
