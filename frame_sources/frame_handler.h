#ifndef FRAME_HANDLER_H
#define FRAME_HANDLER_H

/*!
Abstract frame handler
*/
class IFrameHandler : public std::enable_shared_from_this<IFrameHandler>
{
public:
    /*!
    Handle frame
    \param [in] frameBuffer pointer to framebuffer
    \param [in] w width of frame
    \param [in] h height of frame
    */
    virtual void handleFrame(uint8_t *frameBuffer,
                             int w,
                             int h) = 0;
};

#endif // FRAME_HANDLER_H