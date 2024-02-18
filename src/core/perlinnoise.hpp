/**
 * VGS-Zero - 2D only 16-bits Perlin Noise
 * License under GPLv3: https://github.com/suzukiplan/vgszero/blob/master/LICENSE-VGS0.txt
 * (C)2024, SUZUKI PLAN
 *
 * NOTE: This code is based on the following articles:
 * https://qiita.com/gis/items/ba7d715901a0e572b0e9
 */
#ifndef INCLUDE_PERLINNOISE_HPP
#define INCLUDE_PERLINNOISE_HPP
#include <stdint.h>
#include <string.h>

class PerlinNoise
{
  public:
    struct Context {
        unsigned char p[512];
        double limitX;
        double limitY;
    } ctx;

  private:
    inline double fade(const double t_) { return t_ * t_ * t_ * (t_ * (t_ * 6 - 15) + 10); }
    inline double lerp(const double t_, const double a_, const double b_) { return a_ + t_ * (b_ - a_); }
    inline double grad(const unsigned char hash_, const double u_, const double v_) { return (((hash_ & 1) == 0) ? u_ : -u_) + (((hash_ & 2) == 0) ? v_ : -v_); }
    inline double grad(const unsigned char hash_, const double x_, const double y_, const double z_) { return this->grad(hash_, hash_ < 8 ? x_ : y_, hash_ < 4 ? y_ : hash_ == 12 || hash_ == 14 ? x_
                                                                                                                                                                                                 : z_); }
    inline double floor(double v) { return (double)((long)v); }

    inline double setNoise(double x_, double y_, double z_ = 0.0)
    {
        const size_t x_int{static_cast<size_t>(static_cast<size_t>(this->floor(x_)) & 255)};
        const size_t y_int{static_cast<size_t>(static_cast<size_t>(this->floor(y_)) & 255)};
        const size_t z_int{static_cast<size_t>(static_cast<size_t>(this->floor(z_)) & 255)};
        x_ -= this->floor(x_);
        y_ -= this->floor(y_);
        z_ -= this->floor(z_);
        const double u{this->fade(x_)};
        const double v{this->fade(y_)};
        const double w{this->fade(z_)};
        const size_t a0{static_cast<size_t>(this->ctx.p[x_int] + y_int)};
        const size_t a1{static_cast<size_t>(this->ctx.p[a0] + z_int)};
        const size_t a2{static_cast<size_t>(this->ctx.p[a0 + 1] + z_int)};
        const size_t b0{static_cast<size_t>(this->ctx.p[x_int + 1] + y_int)};
        const size_t b1{static_cast<size_t>(this->ctx.p[b0] + z_int)};
        const size_t b2{static_cast<size_t>(this->ctx.p[b0 + 1] + z_int)};
        return this->lerp(w, this->lerp(v, this->lerp(u, this->grad(this->ctx.p[a1], x_, y_, z_), this->grad(this->ctx.p[b1], x_ - 1, y_, z_)), this->lerp(u, this->grad(this->ctx.p[a2], x_, y_ - 1, z_), this->grad(this->ctx.p[b2], x_ - 1, y_ - 1, z_))), this->lerp(v, this->lerp(u, this->grad(this->ctx.p[a1 + 1], x_, y_, z_ - 1), this->grad(this->ctx.p[b1 + 1], x_ - 1, y_, z_ - 1)), this->lerp(u, this->grad(this->ctx.p[a2 + 1], x_, y_ - 1, z_ - 1), this->grad(this->ctx.p[b2 + 1], x_ - 1, y_ - 1, z_ - 1))));
    }

    inline double setOctaveNoise(const size_t octaves_, double x_, double y_)
    {
        double noise_value{};
        double amp{1.0};
        for (size_t i = 0; i < octaves_; i++) {
            noise_value += this->setNoise(x_, y_) * amp;
            x_ *= 2.0;
            y_ *= 2.0;
            amp *= 0.5;
        }
        return noise_value;
    }

  public:
    PerlinNoise(const unsigned short* random, unsigned short seed)
    {
        this->seed(random, seed);
        this->limitX(64);
        this->limitY(64);
    }

    void seed(const unsigned short* random, unsigned short seed)
    {
        for (int i = 0; i < 256; i++) {
            this->ctx.p[i] = random[seed++] & 0xFF;
            this->ctx.p[256 + i] = this->ctx.p[i];
        }
    }

    void limitX(unsigned short limitX)
    {
        this->ctx.limitX = limitX;
    }

    void limitY(unsigned short limitY)
    {
        this->ctx.limitY = limitY;
    }

    unsigned char noise(unsigned short x, unsigned short y)
    {
        return (unsigned char)(255.0 * (this->setNoise((double)x / this->ctx.limitX, (double)y / this->ctx.limitY) * 0.5 + 0.5));
    }

    unsigned char octave(uint8_t oct, unsigned short x, unsigned short y)
    {
        return (unsigned char)(255.0 * (this->setOctaveNoise(oct, (double)x / this->ctx.limitX, (double)y / this->ctx.limitY) * 0.5 + 0.5));
    }
};

#endif
