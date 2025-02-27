/**
 * DWIN End Stops diagnostic page for PRO UI
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 2.1.3
 * Date: 2023/07/12
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../../inc/MarlinConfigPre.h"

#if ALL(DWIN_LCD_PROUI, HAS_ESDIAG)

#include "endstop_diag.h"

#include "../../../core/types.h"
#include "../../marlinui.h"
#include "dwin.h"
#include "dwin_popup.h"

#if HAS_FILAMENT_SENSOR
  #include "../../../feature/runout.h"
#endif

#if HAS_BED_PROBE
  #include "../../../module/probe.h"
#endif

ESDiag esDiag;

void draw_es_label(FSTR_P const flabel=nullptr) {
  DWINUI::cursor.x = 40;
  if (flabel) DWINUI::drawString(F(flabel));
  DWINUI::drawString(F(": "));
  DWINUI::moveBy(0, 25);
}

void draw_es_state(const bool is_hit) {
  const uint8_t LM = 130;
  DWINUI::cursor.x = LM;
  dwinDrawRectangle(1, hmiData.colorPopupBg, LM, DWINUI::cursor.y, LM + 100, DWINUI::cursor.y + 20);
  is_hit ? DWINUI::drawString(RGB(31,31,16), F(STR_ENDSTOP_HIT)) : DWINUI::drawString(RGB(16,63,16), F(STR_ENDSTOP_OPEN));
  DWINUI::moveBy(0, 25);
}

void ESDiag::draw() {
  title.draw(GET_TEXT_F(MSG_ENDSTOP_TEST));
  DWINUI::clearMainArea();
  drawPopupBkgd();
  DWINUI::drawButton(BTN_Continue, 86, 250);
  DWINUI::cursor.y = 80;
  #define ES_LABEL(S) draw_es_label(F(STR_##S))
  TERN_(USE_X_MIN, ES_LABEL(X_MIN)); TERN_(USE_X_MAX, ES_LABEL(X_MAX));
  TERN_(USE_Y_MIN, ES_LABEL(Y_MIN)); TERN_(USE_Y_MAX, ES_LABEL(Y_MAX));
  TERN_(USE_Z_MIN, ES_LABEL(Z_MIN)); TERN_(USE_Z_MAX, ES_LABEL(Z_MAX));
  TERN_(HAS_FILAMENT_SENSOR, draw_es_label(F(STR_FILAMENT)));
  TERN_(HAS_FILAMENT_MOTION, draw_es_label(F(STR_FILAMENT_MOTION)));
  update();
}

void ESDiag::update() {
  DWINUI::cursor.y = 80;
  #define ES_REPORT(S) draw_es_state(READ(S##_PIN) == S##_ENDSTOP_HIT_STATE)
  TERN_(USE_X_MIN, ES_REPORT(X_MIN)); TERN_(USE_X_MAX, ES_REPORT(X_MAX));
  TERN_(USE_Y_MIN, ES_REPORT(Y_MIN)); TERN_(USE_Y_MAX, ES_REPORT(Y_MAX));
  TERN_(USE_Z_MIN, ES_REPORT(Z_MIN)); TERN_(USE_Z_MAX, ES_REPORT(Z_MAX));
  #if HAS_FILAMENT_SENSOR
    draw_es_state(READ(FIL_RUNOUT1_PIN) == FIL_RUNOUT1_STATE);

    #if HAS_FILAMENT_MOTION
      draw_es_state(READ(FIL_MOTION1_PIN) == FIL_MOTION1_STATE);
    #endif
  #endif
  dwinUpdateLCD();
}

#endif // DWIN_LCD_PROUI && HAS_ESDIAG
