#include "HookedMethods.hpp"

extern settings::Boolean engine_pred;
namespace hooked_methods
{
// Update Prediction, Used by warp/Doubletap aswell
void UpdatePred()
{
    if (isHackActive() && g_IEngine->IsInGame() && CE_GOOD(LOCAL_E) && engine_pred)
    {
        int signon_state        = *(int *) (*(unsigned *) &g_IBaseClientState + 304);
        int m_nDeltaTick        = *(int *) (*(unsigned *) &g_IBaseClientState + 408);
        int lastoutgoingcommand = *(int *) (*(unsigned *) &g_IBaseClientState + offsets::lastoutgoingcommand());
        int chokedcommands      = *(int *) (*(unsigned *) &g_IBaseClientState + offsets::lastoutgoingcommand() + 4);
        int last_command_ack    = *(int *) (*(unsigned *) &g_IBaseClientState + offsets::lastoutgoingcommand() + 8);

        // Only run if fully connected
        if (signon_state == 6 && m_nDeltaTick > 0)
            g_IPrediction->Update(m_nDeltaTick ? m_nDeltaTick + 1 : 0, m_nDeltaTick > 0, last_command_ack, lastoutgoingcommand + chokedcommands);
    }
}
DEFINE_HOOKED_METHOD(Think, void, IToolFrameworkInternal *_this, bool finaltick)
{
    UpdatePred();
    return original::Think(_this, finaltick);
}

} // namespace hooked_methods
