#ifndef GLTIMER_HPP
# define GLTIMER_HPP

# include <glad/glad.h>

class GLTimer
{
    public:
        GLTimer();
        ~GLTimer();
        void update();
        float getDeltaTime() const;
        void reset();
    private:
        double m_lastTime;
        double m_deltaTime;
};

#endif