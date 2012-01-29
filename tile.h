#ifndef TILE_H
#define TILE_H

#include <string>
#include <vector>
#include <QString>

using namespace std;

class Tile
{
public:
    int scor;

    Tile(string type);
    vector<vector<char> > data;

    string getAsText() const;
    QString getAsQString() const { return QString(getAsText().c_str()); }
    int getHeight() const { return data.size(); }
    int getWidth() const { return data.size() > 0 ? data[0].size() : 0; }
    int score() const;
    void reflectTile();
    void rotateTile();
};

ostream & operator<<(ostream &, const Tile &);

#endif
