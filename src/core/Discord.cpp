#ifndef BC_RETRO
#include "Discord.h"

#include "Logger.h"
#include "Settings.h"

#include "../../lib/discord_game_sdk/discord_game_sdk.h"
#include <stdio.h>
#include <stdlib.h>

struct Application app;

void DISCORD_CALLBACK
bc_discord_UpdateActivityCallback(void *data, enum EDiscordResult result) {
    bc_log("%d\n", result);
}

void bc_discord_activity_update(const char *details, const char *state) {
    if (app.activity == NULL)
        return;

    struct DiscordActivity activity;
    struct DiscordActivityAssets activityAssets;
    memset(&activity, 0, sizeof(activity));
    memset(&activityAssets, 0, sizeof(activityAssets));

    snprintf(activityAssets.large_image, sizeof(activityAssets.large_image),
             "%s", "logo_betacraft_1024");
    snprintf(activityAssets.large_text, sizeof(activityAssets.large_text), "%s",
             "Download at betacraft.uk");
    snprintf(activity.details, sizeof(activity.details), "%s", details);
    snprintf(activity.state, sizeof(activity.state), "%s", state);

    activity.assets = activityAssets;
    activity.application_id = DISCORD_CLIENT_ID;

    app.activity->update_activity(app.activity, &activity, &app,
                                  bc_discord_UpdateActivityCallback);
}

int bc_discord_init() {
    if (DISCORD_CLIENT_ID == 0)
        return 0;

    memset(&app, 0, sizeof(app));

    IDiscordCoreEvents events;
    memset(&events, 0, sizeof(events));

    struct IDiscordActivityEvents activity_events;
    memset(&activity_events, 0, sizeof(activity_events));

    struct DiscordCreateParams params;
    params.client_id = DISCORD_CLIENT_ID;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.events = &events;
    params.activity_events = &activity_events;
    params.event_data = &app;

    enum EDiscordResult create =
        DiscordCreate(DISCORD_VERSION, &params, &app.core);

    bc_settings *settings = bc_settings_get();
    int toggled = settings->discord && create == DiscordResult_Ok;
    free(settings);

    // If discord instance is running
    if (toggled) {
        app.activity = app.core->get_activity_manager(app.core);
    }

    return toggled;
}

void bc_discord_loop() { app.core->run_callbacks(app.core); }

void bc_discord_stop() { app.core->destroy(app.core); }
#endif