#include "tile.h"
#include <iostream>

using namespace std;

Tile::Tile(string src)
{
    data.resize(1);
    _score = 0;
    for (size_t i = 0; i < src.size(); ++i)
    {
        if(src[i] == '|')
        {
            data.resize(data.size() + 1);
        }
        else
        {
            data.back().push_back(src[i]);
            if (src[i] != '0')
            {
                ++_score;
            }
        }
    }
}

void Tile::rotate()
{
    vector<vector<char> > newdata(width(), vector<char>(height(),'0'));
    for (int i = 0; i < height(); ++i)
    {
        for (int j = 0; j < width(); ++j)
        {
            newdata[j][height() - i - 1] = data[i][j];
        }
    }

    data.swap(newdata);
}

void Tile::reflect()
{
    vector<vector<char> > newdata(height(), vector<char>(width(), '0'));
    for (int i = 0; i < height(); ++i)
    {
        for (int j = 0; j < width(); ++j)
        {
            newdata[i][width() - j - 1] = data[i][j];
        }
    }

    data.swap(newdata);
}

string Tile::getAsText() const
{
    string result;
    for (int i = 0; i < height(); ++i)
    {
        for (int j = 0; j < width(); ++j)
        {
            result.push_back(data[i][j]);
        }

        if (i < height() - 1)
        {
            result.push_back('|');
        }
    }

    return result;
}

ostream &operator<<(ostream &to, const Tile &what)
{
    for (size_t i = 0; i < what.data.size(); ++i)
    {
        for (size_t j = 0; j < what.data[i].size(); ++j)
        {
            to << what.data[i][j];
        }

        to << endl;
    }

    return to;
}
