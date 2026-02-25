(function () {
            /* ---- Title: letter-by-letter drop ---- */
            var titleText = 'Smart House';
            var titleEl = document.getElementById('mainTitle');
            var baseDelay = 700; /* ms after page load */
            for (var i = 0; i < titleText.length; i++) {
                var span = document.createElement('span');
                if (titleText[i] === ' ') {
                    span.className = 'letter-space';
                } else {
                    span.className = 'letter';
                    span.textContent = titleText[i];
                    span.style.animationDelay = (baseDelay + i * 60) + 'ms';
                }
                titleEl.appendChild(span);
            }

            /* ---- Subtitle: typewriter ---- */
            var subText = 'Panel de Control';
            var subEl = document.getElementById('subTitle');
            var subBaseDelay = baseDelay + titleText.length * 60 + 300;

            for (var j = 0; j < subText.length; j++) {
                var ch = document.createElement('span');
                if (subText[j] === ' ') {
                    ch.className = 'char-space';
                } else {
                    ch.className = 'char';
                    ch.textContent = subText[j];
                    ch.style.animationDelay = (subBaseDelay + j * 55) + 'ms';
                }
                subEl.appendChild(ch);
            }

            /* ---- Status dots: sequential pop ---- */
            var dotDelay = subBaseDelay + subText.length * 55 + 200;
            var dots = [
                document.getElementById('dot1'),
                document.getElementById('dot2'),
                document.getElementById('dot3')
            ];
            dots.forEach(function (dot, idx) {
                dot.style.animationDelay = (dotDelay + idx * 150) + 'ms';
                /* After entrance, switch to infinite pulse */
                setTimeout(function () {
                    dot.style.animation = 'none';
                    /* Force reflow */
                    void dot.offsetWidth;
                    /* Remove inline animation so .live class takes over */
                    dot.style.animation = '';
                    dot.classList.add('live');
                }, dotDelay + idx * 150 + 500);
            });

            /* ---- Grid items: staggered card reveal ---- */
            var gridDelay = dotDelay + 600;
            var items = document.getElementById('gridBotones').children;
            for (var k = 0; k < items.length; k++) {
                (function (el, delay) {
                    setTimeout(function () {
                        el.style.animationDelay = '0ms';
                        el.classList.add('revealed');
                    }, delay);
                })(items[k], gridDelay + k * 80);
            }

        })();
