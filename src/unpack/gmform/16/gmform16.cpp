#include "gmform/16/gmform16.h"
#include "binaryreader.h"

GmForm16::GmForm16(BinaryReader& br)
    : header_(br)
    , general_(br)
    , options_(br)
    , language_(br)
    , extensions_(br)
    , sounds_(br)
    , audioGroup_(br)
    , sprites_(br)
    , backgrounds_(br)
    , paths_(br)
    , scripts_(br)
    , globals_(br)
    , shaders_(br)
    , fonts_(br)
    , timelines_(br)
    , objects_(br)
    , rooms_(br)
    , datafiles_(br)
    , texturePages_(br)
    , code_(br)
    , variables_(br)
    , functions_(br)
    , strings_(br)
    , textures_(br)
    , audio_(br)
{
    functions_.postInit(*this);
    variables_.postInit(*this);
    code_.postInit(*this);
}

GmForm16::~GmForm16()
{}
