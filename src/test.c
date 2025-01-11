void blit_rect(APTR dst_base,
               UWORD dst_stride_b,
               UWORD dst_x,
               UWORD dst_y,
               APTR mask_base,
               UWORD mask_stride_b,
               UWORD mask_x,
               UWORD mask_y,
               UWORD width,
               UWORD height,
               BOOL set_bits) {
  UWORD start_x_word = dst_x >> 4;
  UWORD end_x_word = ((dst_x + width) + 0xF) >> 4;
  UWORD width_words = end_x_word - start_x_word;
  UWORD word_offset = dst_x & 0xF;

  UWORD dst_mod_b = dst_stride_b - (width_words * kBytesPerWord);
  UWORD mask_mod_b = mask_stride_b - (width_words * kBytesPerWord);

  ULONG dst_start_b = (ULONG)dst_base + (dst_y * dst_stride_b) + (start_x_word * kBytesPerWord);
  ULONG mask_start_b = (ULONG)mask_base + (mask_y * mask_stride_b) + (start_x_word * kBytesPerWord);

  UWORD left_word_mask = (UWORD)(0xFFFFU << (word_offset + MAX(0, 0x10 - (word_offset + width)))) >> word_offset;
  UWORD right_word_mask;

  if (width_words == 1) {
    right_word_mask = left_word_mask;
  }
  else {
    right_word_mask = 0xFFFFU << MIN(0x10, ((start_x_word + width_words) << 4) - (dst_x + width));
  }

  UWORD minterm = 0xA;

  if (mask_base) {
    minterm |= set_bits ? 0xB0 : 0x80;
  }
  else {
    minterm |= set_bits ? 0xF0 : 0x00;
  }

  gfx_wait_blit();

  // A = Mask of bits inside copy region
  // B = Optional bitplane mask
  // C = Destination data (for region outside mask)
  // D = Destination data
  custom.bltcon0 = BLTCON0_USEC | BLTCON0_USED | (mask_base ? BLTCON0_USEB : 0) | minterm;
  custom.bltcon1 = 0;
  custom.bltbmod = mask_mod_b;
  custom.bltcmod = dst_mod_b;
  custom.bltdmod = dst_mod_b;
  custom.bltafwm = left_word_mask;
  custom.bltalwm = right_word_mask;
  custom.bltadat = 0xFFFF;
  custom.bltbpt = (APTR)mask_start_b;
  custom.bltcpt = (APTR)dst_start_b;
  custom.bltdpt = (APTR)dst_start_b;
  custom.bltsize = (height << BLTSIZE_H0_SHF) | width_words;
}